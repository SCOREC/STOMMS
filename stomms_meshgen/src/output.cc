#include "output.h"

/***********************************************/
// Class: StommsOutput
/***********************************************/
StommsOutput::StommsOutput(const StommsMesh& m, const GridFieldData& gridData):mesh(m), gridFieldData(gridData)
{
  std::cout << "============ Output Writing Starts ============\n";

  // Step 1: Get underlying Simmetrix Model stored in mesh meta data.
  MeshMetaData meshMetaData = mesh.getMeshMetaData();
  StommsModel stommsModel = meshMetaData.getStommsModel();
  Model mdl = stommsModel.getModel();
  simModel = mdl.getSimModel();

  // Step 2: Get Simmetrix mesh and its dimension.
  simMesh = mesh.getSimMesh();
  meshDim = mesh.getMeshDim();

  // Step 3: Get the mesh planes data and geometric planes data.
  geometricPlanes = stommsModel.getPlanes();
  planes = mesh.getMeshDataOnPlanes();
 
  // Step 4: Write Simmetrix meshes to OmegaH meshes.
  writeOmegahMeshes(); 

  // Step 5: Write output files
  if (outputVtk)
    writeVtkFromOmegah(omegahMeshes);
 
  if (outputGmsh)
    writeGmshFromOmegah(omegahMeshes);    
  
  // Step 6: Write data into adios2 file.
  writeAdiosFile();

  // Step 7: Read adios2 file for verification of data.
  // Don't need it unless we are debugging.
  // readAdiosFile();
}

// Destructor
StommsOutput::~StommsOutput()
{
  M_release(simMesh);
  for (int i = 0; i < planes.size(); i++)
  {
    PlaneMeshData p = planes[i];
    M_release(p.getMesh());
  }
  MD_deleteMeshDataId(transformCoordinates);
  GM_release(simModel);
}

// Function to iterate over all the planes and creates omegah meshes
// for each individual plane. And store them in container (omegahMeshes).
void StommsOutput::writeOmegahMeshes()
{
  // If its 2D mesh, go over each plane one by one and write omegah mesh 
  // for each plane. 
  if (meshDim == 2)
  {
    for (int i = 0; i < planes.size(); i++)
    {
      PlaneMeshData p = planes[i];
      std::cout << "          Plane # " << i << "          \n";
      auto mesh = simMesh2Omegah2D(p);
      omegahMeshes.push_back(mesh);
      std::cout << "Omegah mesh for plane # " << i << " is written\n";
      std::cout << "===============================================\n";
    }
  }
  else if (meshDim == 3) // For 3D mesh, will have a single omegah mesh.
  {
    auto mesh = simMesh2Omegah3D();
    omegahMeshes.push_back(mesh);
    std::cout << "3D Omegah mesh has been written\n";
  }
  std::cout << "OmegaH mesh files writing: Done\n";
}

// To make sure, vertex indices start from 0 to nVertices-1. 
void StommsOutput::setMeshIndices(pMesh& m)
{
  VIter vIter = M_vertexIter(m);
  int vNum = 0;
  while (pVertex v =  VIter_next(vIter))
  {
    EN_setID(v,vNum);
    vNum++;
  }
  VIter_delete(vIter);
}

// Transformation of 3D Cartesian (XYZ) to 2D Cylindrical (RZ) for planer meshes.
void StommsOutput::attachCoordinateTransformationData(pMesh& m)
{
  double xyz[3];
  VIter vIter = M_vertexIter(m);
  while (pVertex v =  VIter_next(vIter))
  {
    V_coord(v, xyz);

    // Convert 3D cartesian to 2D Planer (R_Z) cylindrical
    double r = sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1]);
    double *vData = new double[3];
    vData[0] = r;
    vData[1] = xyz[2];
    vData[2] = 0.0;

    // Attach the data to the mesh vertices.
    EN_attachDataPtr((pEntity)v, transformCoordinates, (void*)vData);
    //delete[] vData;
  }  
  VIter_delete(vIter);
}

// Give Plane as input to this fucntion
Omega_h::Mesh StommsOutput::simMesh2Omegah2D(const PlaneMeshData& plane)
{
  // Step 1: Get the Simmetrix mesh enteties on each plane and construct SimMesh m
  // to feed into Omegah APIs.
  PlaneMeshData p = plane;
  pMesh m = p.getMesh();

  // Step 2: Make sure vertex indices are consistent and starts from 0 on each plane
  setMeshIndices(m);
  attachCoordinateTransformationData(m);

  // Step 3: Create an object of Omegah mesh. 
  auto lib = Omega_h::Library(NULL, NULL);
  auto comm = lib.world();

  auto mesh = Omega_h::meshsim::read(&m, std::string(""), comm, &transformCoordinates);
  return mesh; 
}

// Given 3D Simmetrix mesh, convert it to Omegah mesh
Omega_h::Mesh StommsOutput::simMesh2Omegah3D()
{
  // Step 1: Create an object of Omegah mesh.  
  auto lib = Omega_h::Library(NULL, NULL);
  auto comm = lib.world();

  auto mesh = Omega_h::meshsim::read(&simMesh, std::string(""), comm);

  return mesh;
}

void StommsOutput::writeAdiosFile()
{
  // Step 1: Create a file for adios2 output.
  Omega_h::filesystem::create_directory(adiosOutFileName);

  // Step 2: Create adios2 object and io. And set the filename.
  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("stommsWriter");
  std::string filename=adiosOutFileName.c_str();

  // Step 3: Start the writer engine
  adios2::Engine writer = io.Open(filename, adios2::Mode::Write);
  writer.BeginStep();

  // Step 4: Write git hash to the adios2 file. GIT_HASH variable coming from Cmake. 
  const std::string versionNumber = STOMMS_GIT_HASH;
  adios2::Variable<std::string> versionVariable = io.DefineVariable<std::string>("StommsVersion");
  writer.Put(versionVariable, versionNumber);

  // Step 5: Iterate over omegah meshes for planes and write them to adios2.
  // See namespace adios in omegah for more details.
  for (int i = 0; i < omegahMeshes.size(); i++)
  {
    std::string meshName = "stommsMesh/planes/" + std::to_string(i)+"/";
    Omega_h::Mesh* meshPlane = &omegahMeshes[i];
    Omega_h::adios::write_mesh(io, writer, meshPlane, meshName);

    // Step 5.1: Write physics classification information in adios2 file.
    writePhysicsClassification(io, writer, i);
  
    // Step 5.2: Write Model Adjacency Information.
    writeModelAdjacency(io, writer, i);
  }

  // Step 6: Write input grid information.
  writeGridInformation(io, writer);

  // Step 7: End the writer engine.
  writer.EndStep();
  writer.Close();

  std::cout << "Adios2 file written\n";
}

// Need this function for verification of mesh data written to adios2 file.
void StommsOutput::readAdiosFile()
{
  std::string filename = "stommsMesh.bp";  // adios2 file to read

  // Step 1: Create adios2 object and io.
  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("stommsReader");

  // Step 2: Start the reader engine
  adios2::Engine reader = io.Open(filename, adios2::Mode::Read);
  reader.BeginStep();

  // Step 3: Read and print all the variables from adios2 file
  auto available_vars = io.AvailableVariables();
  std::cout << "========== Reading output file: " << filename << " ==========\n";
  
  std::cout << "Number of variables in file = " << available_vars.size() << "\n";
  for (const auto& var_pair : available_vars) {
    std::cout << "Variable name: " << var_pair.first << "\n";
  }
  reader.EndStep();

  // Step 4: Read Omega_h meshes from adios2 file. 
  // The data printing out from the loop is just for verification.
  auto lib = Omega_h::Library(NULL, NULL);
  auto world = lib.world();
  std::vector <Omega_h::Mesh> meshFromAdiosFile;
  for (int i = 0; i < omegahMeshes.size(); i++)
  {
    std::string meshName = "stommsMesh/planes/" + std::to_string(i)+"/";
    Omega_h::Mesh meshFromFile = Omega_h::adios::read(adiosOutFileName, &lib, meshName);
    meshFromAdiosFile.push_back(meshFromFile);
    std::cout << "========== Reading Mesh: " << meshName << " ========== \n";
    const auto coords = meshFromFile.coords();
    std::cout << "# of vertices = " << meshFromFile.nverts() << "\n";
    std::cout << "Location of vertex 0 on plane # " << i << " : " << coords[0] << " , " << coords[1] << "\n";
    std::cout << "# of edges = " << meshFromFile.nedges() << "\n";
    std::cout << "# of elements = " << meshFromFile.nelems() << "\n";
  }

  // Step 5: Close the reader engine.
  reader.Close();
}

// Free functions to write output files.
// Write vtk file from Omegah (2D or 3D)
void writeOmegah2Vtk(Omega_h::Mesh mesh, std::string meshName)
{
  int dim = mesh.dim();
  Omega_h::vtk::write_vtu(meshName, &mesh, dim);
}

// Given a vector of 2D meshes, or single 3D mesh, write vtk files
void writeVtkFromOmegah(const std::vector <Omega_h::Mesh>& omegahMeshPlanes)
{
  // 3D Mesh
  if (omegahMeshPlanes.size() == 1 && omegahMeshPlanes[0].dim() == 3)
  {
    std::string meshName = "mesh_3d.vtk";
    Omega_h::Mesh m = omegahMeshPlanes[0];
    writeOmegah2Vtk(m,meshName);
  }

  //2D Planer Meshes
  else
  {
    for (int i = 0; i < omegahMeshPlanes.size(); i++)
    {
      Omega_h::Mesh m = omegahMeshPlanes[i];
      std::string meshName = "meshPlane_" + std::to_string(i) + ".vtk";
      writeOmegah2Vtk(m,meshName);
    }
  }
}

// Given a vector of 2D meshes, or single 3D mesh, write gmsh files
void writeGmshFromOmegah(const std::vector <Omega_h::Mesh>& omegahMeshPlanes)
{
  // 3D Mesh
  if (omegahMeshPlanes.size() == 1 && omegahMeshPlanes[0].dim() == 3)
  {
    std::string meshName = "mesh_3d.msh";
    Omega_h::Mesh m = omegahMeshPlanes[0];
    Omega_h::gmsh::write(meshName, &m);
  }

  // 2D Planer Meshes
  else
  {
    for (int i = 0; i < omegahMeshPlanes.size(); i++)
    {
      Omega_h::Mesh m = omegahMeshPlanes[i];
      std::string meshName = "meshPlane_" + std::to_string(i) + ".msh";
      Omega_h::gmsh::write(meshName, &m);
    }
  }
}

// Function to write physics classification in given adios2 file.
void StommsOutput::writePhysicsClassification(adios2::IO& io, adios2::Engine& writer, int planeIndex)
{
  // Step 1: Set the mesh as pre-name for variables.
  std::string name = "stommsMesh/planes/" + std::to_string(planeIndex) + "/physicsClassification/";

  // Step 2: Get a map of model faces with physics type as a key,and 
  // then write the array of model faces corresponding to that key.
  std::vector <Face> modelFaces = geometricPlanes[planeIndex].modelFaces;
  gfPhysics = modelFaceClassification(modelFaces);
  for (auto& faceMap: gfPhysics)
  {
    std::string physicsType = getFaceType(faceMap.first);
    std::string varName = name + "face/" + physicsType;
    writeAdios2Array(io, writer, faceMap.second, 1, varName);
  }

  // Step 3: First index the curves as needed in XGC. and then get a map 
  // of model curves with physics type as a key,and then write the array 
  // of model curves corresponding to that key.
  std::vector <Flux> allFluxCurves = geometricPlanes[planeIndex].fluxCurves;
  curvesSortedByPsi = curveIndexing(allFluxCurves);
  std::vector <Edge> wallEdges = geometricPlanes[planeIndex].wallEdges; 

  std::map<std::string, CurvesGroup> curveTypes = curveClassification(curvesSortedByPsi, wallEdges);
  for (auto& curveMap: curveTypes)
  {
    CurvesGroup curveGroup = curveMap.second; // closed, open, separatrix, wall
    std::string groupName = name + "edge/" + curveMap.first;
    std::string varName = groupName + "/fluxIds";
    writeAdios2Array(io, writer, curveGroup.flxId, 1, varName);
    varName = groupName + "/psi";
    writeAdios2Array(io, writer, curveGroup.psi, 1, varName); 
    varName = groupName + "/modelEdges/range";
    writeAdios2Array(io, writer, curveGroup.modelEdgesRange, 1, varName);
    varName = groupName + "/modelEdges/data";
    writeAdios2Array(io, writer, curveGroup.modelEdgesVector, 1, varName); 
  }
   
  // Step 4: Get a map of model vertices with physics type as a key,
  // and then write the model vertex corresponding to that key.
  Vertex oPoint = geometricPlanes[planeIndex].oPoint;
  std::vector <Vertex> xPoints = geometricPlanes[planeIndex].getXPointsOnPlane();
  gvPhysics = modelVertexClassification(oPoint, xPoints);
  for (auto& vertexMap: gvPhysics)
  {
    std::string physicsType = getVertexType(vertexMap.first);
    std::string varName = name + "vertex/criticalPoints/" + physicsType;
    std::vector <int> vMap = vertexMap.second;
    for (int i = 0; i < vMap.size(); i++)
    {
      std::string vName = varName + "/" + std::to_string(i+1);
      writeAdios2Value(io, writer, vMap[i], vName);
    }
  }
}

// Function to write all the model adjacencies in given adios2 file.
void StommsOutput::writeModelAdjacency(adios2::IO& io, adios2::Engine& writer, int planeIndex)
{
  const Plane& plane = geometricPlanes[planeIndex];
  std::string name, groupName, varName;
  Adj adj;

  // Step 1: Write adjacencies on vertices (edges & faces)
  // Step 1.1: Set variable pre name and write vertex ids 
  name = "stommsMesh/planes/" + std::to_string(planeIndex) + "/modelAdj/0/";
  adj = getAdjacency(0, plane); // Vertex adjacencies
  varName = name + "vertexIds";
  writeAdios2Array(io, writer, adj.entId, 1, varName);

  // Step 1.2: Write vertex to edge adjacencies for all model vertices.
  groupName = name + "toEdge/";
  varName = groupName + "range";
  writeAdios2Array(io, writer, adj.rangeVector_1, 1, varName);
  varName = groupName + "data";
  writeAdios2Array(io, writer, adj.adjVector_1, 1, varName);

  // Step 1.3: Write vertex to face adjacencies for all model vertices.
  groupName = name + "toFace/";
  varName = groupName + "range";
  writeAdios2Array(io, writer, adj.rangeVector_2, 1, varName);
  varName = groupName + "data";
  writeAdios2Array(io, writer, adj.adjVector_2, 1, varName);

  // Step 2: Write adjacencies on edges (vertices & faces)
  // Step 2.1: Set variable pre name and write edge ids
  name = "stommsMesh/planes/" + std::to_string(planeIndex) + "/modelAdj/1/";
  adj = getAdjacency(1, plane); // edge adjacencies
  varName = name + "edgeIds";
  writeAdios2Array(io, writer, adj.entId, 1, varName);

  // Step 2.2: Write edge to vertex adjacencies for all model edges.
  groupName = name + "toVertex/";
  varName = groupName + "range";
  writeAdios2Array(io, writer, adj.rangeVector_1, 1, varName);
  varName = groupName + "data";
  writeAdios2Array(io, writer, adj.adjVector_1, 1, varName);

  // Step 2.3: Write edge to face adjacencies for all model edges.
  groupName = name + "toFace/";
  varName = groupName + "range";
  writeAdios2Array(io, writer, adj.rangeVector_2, 1, varName);
  varName = groupName + "data";
  writeAdios2Array(io, writer, adj.adjVector_2, 1, varName);

  // Step 3: Write adjacencies on faces (vertices & edges)
  // Step 3.1: Set variable pre name and write face ids
  name = "stommsMesh/planes/" + std::to_string(planeIndex) + "/modelAdj/2/";
  adj = getAdjacency(2, plane); // edge adjacencies
  varName = name + "faceIds";
  writeAdios2Array(io, writer, adj.entId, 1, varName);

  // Step 3.2: Write face to vertex adjacencies for all model faces.
  groupName = name + "toVertex/";
  varName = groupName + "range";
  writeAdios2Array(io, writer, adj.rangeVector_1, 1, varName);
  varName = groupName + "data";
  writeAdios2Array(io, writer, adj.adjVector_1, 1, varName);

  // Step 3.3: Write face to edge adjacencies for all model faces.
  groupName = name + "toEdge/";
  varName = groupName + "range";
  writeAdios2Array(io, writer, adj.rangeVector_2, 1, varName);
  varName = groupName + "data";
  writeAdios2Array(io, writer, adj.adjVector_2, 1, varName);
}

// Function to write magnetic field information from background grid to adios2 file.
void StommsOutput::writeGridInformation(adios2::IO& io, adios2::Engine& writer)
{
  // Step 1: Declare the variables and names of the variables needed in adios2 file
  std::string name, varName;
  name  = "stommsMesh/fieldGridData/";

  // Step 2: Check if the GridFieldData object has the grid information populated. If not, return.
  // If yes, start writing the data in adios2 file.
  std::vector <double> psiField = gridFieldData.getDoubleFieldData(FieldType::Psi);
  if (!psiField.size())
    return;

  // Step 3: Read the grid points (r and z) and write them in adios2 file.
  std::vector <double> rPoints = gridFieldData.getRPoints();
  std::vector <double> zPoints = gridFieldData.getZPoints();
  varName = name + "gridPointsR";
  writeAdios2Array(io, writer, rPoints, 1, varName);
  varName = name + "gridPointsZ";
  writeAdios2Array(io, writer, zPoints, 1, varName);

  // Step 4: Write psi Grid Data to adios2 file
  varName = name + "psiGrid";
  writeAdios2Array(io, writer, psiField, zPoints.size(), varName);

  // Step 5: Write psi and poloidal current arrays
  writeFieldArraysToGrid(io, writer, name);

  // Step 6: Write physical boundaries data.
  writePhysicalDataToGrid(io, writer, name);

  // STep 7: Write spline data to the grid.
  writeSplinesDataToGrid(io, writer, name);
}

// Function to write field arrays from input grid information to adios2 grid information.
void StommsOutput::writeFieldArraysToGrid(adios2::IO& io, adios2::Engine& writer, std::string& name)
{
  std::string varName;  // name of the variable to be written to adios2  

  // Step 1: Read psi array and write it to adios2 file
  std::vector <double> psiArray = gridFieldData.getPsiArray();
  varName = name + "psi";
  writeAdios2Array(io, writer, psiArray, 1, varName);
   
  // Step 2: Read poloidal current and write it to adios2 file
  std::vector <double> currentArray = gridFieldData.getPoloidalCurrentArray();
  varName = name + "poloidalCurrent";
  writeAdios2Array(io, writer, currentArray, 1, varName);
}

// Function to write physical coordinates of the entities from input grid 
// information to adios2 grid information.
void StommsOutput::writePhysicalDataToGrid(adios2::IO& io, adios2::Engine& writer, std::string& name)
{
  std::string varName;  // name of the variable to be written to adios2

  // Step 1: Write bounding box to the adios2 file
  // rMin = box[0], zMin = box[1], rMax = box[2], zMax = box[3]
  std::vector <double> box = gridFieldData.getDomainBox();
  varName = name + "DomainBox";
  writeAdios2Array(io, writer, box, 1, varName);

  // Step 2: Write wall curve from the eqdsk file. Might be different from 
  // the wall curve we have used in processing since it could come from 
  // external file instead of eqdsk file.
  std::vector <double> rLimiterPoints = gridFieldData.getLimiterPointsR();
  std::vector <double> zLimiterPoints = gridFieldData.getLimiterPointsZ();
  varName = name + "limiterPointsR";
  writeAdios2Array(io, writer, rLimiterPoints, 1, varName);
  varName = name + "limiterPointsZ";
  writeAdios2Array(io, writer, zLimiterPoints, 1, varName);

  // Step 3: Write seperatrix points from eqdsk file. This is the different 
  // from the separatrix curve points we traced. These are taken directly 
  // from eqdsk utility code. We might not need them in adios2 file.
  std::vector <double> rBdryPoints = gridFieldData.getBdryPointsR();
  std::vector <double> zBdryPoints = gridFieldData.getBdryPointsZ();
  varName = name + "boundaryPointsR";
  writeAdios2Array(io, writer, rBdryPoints, 1, varName);
  varName = name + "boundaryPointsZ";
  writeAdios2Array(io, writer, zBdryPoints, 1, varName);
}

// Function to write spline data to the adios2 file.
void StommsOutput::writeSplinesDataToGrid(adios2::IO& io, adios2::Engine& writer, std::string& name)
{
  std::string varName;  // name of the variable to be written to adios2

  // Step 1: Get the Psi spline coefficients and its array shape.
  std::vector <int> arrayShape = gridFieldData.getPsiSplineShape();
  std::vector <double> psiSplineCoefficients = gridFieldData.getPsiSplineCoefficients();
  varName = name + "psiSplineCoefficients";
  writeAdios2MultiDimArray(io, writer, psiSplineCoefficients, arrayShape, varName);

  // Step 2: Get the poloidal current spline coefficients and its array shape.
  arrayShape = gridFieldData.getCurrentSplineShape();
  std::vector <double> currentSplineCoefficients = gridFieldData.getCurrentSplineCoefficients();
  varName = name + "poloidalCurrentSplineCoefficients";
  writeAdios2MultiDimArray(io, writer, currentSplineCoefficients, arrayShape, varName);
}
