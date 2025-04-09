#include "output.h"

StommsOutput::StommsOutput(const StommsMesh& m):mesh(m)
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

  // Step 3: Get the planes data.
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

  // Step 7: Read adios2 file for verification of data
  readAdiosFile();
}

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

  auto mesh = Omega_h::meshsim::read(&m, std::string(""), comm);

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
  const std::string versionNumber = GIT_HASH;
  adios2::Variable<std::string> versionVariable = io.DefineVariable<std::string>("Version");
  writer.Put(versionVariable, versionNumber);

  // Step 5: Iterate over omegah meshes for planes and write them to adios2.
  // See namespace adios in omegah for more details.
  for (int i = 0; i < omegahMeshes.size(); i++)
  {
    std::string meshName = "meshPlane_" + std::to_string(i);
    Omega_h::Mesh* meshPlane = &omegahMeshes[i];
    Omega_h::adios::write_mesh(io, writer, meshPlane, meshName);
  }

  // Step 6: End the writer engine.
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
    std::string meshName = "meshPlane_" + std::to_string(i);
    Omega_h::Mesh meshFromFile = Omega_h::adios::read(adiosOutFileName, &lib, meshName);
    meshFromAdiosFile.push_back(meshFromFile);
    std::cout << "========== Reading Mesh: " << meshName << " ========== \n";
    const auto coords = meshFromFile.coords();
    std::cout << "# of vertices = " << meshFromFile.nverts() << "\n";
    std::cout << "Location of vertex 0 on plane # " << i << " : " << coords[0] << " , " << coords[1] << "\n";
    std::cout << "# of edges = " << meshFromFile.nedges() << "\n";
    std::cout << "# of elements = " << meshFromFile.nelems() << "\n";
  }

  // Step 5: Close the reder engine.
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

// To debug the routines for output writing - Once development is done, we 
// can get rid of these debug functions.
void debugMesh(std::vector <pVertex> v, std::vector <pEdge> e, 
          std::vector <pFace> f, std::vector <pRegion> r) {
  
  FILE* fp =fopen("meshInfo.txt","w");

  fprintf(fp, "Number of Mesh Vertices = %d\n", v.size());
  double xyz[3];
  // Iterate over vertices
  for (int i = 0; i < v.size(); i++)
  {
    V_coord(v[i], xyz);
    EN_setID(v[i],i);
    fprintf(fp, "%d %lf %lf %lf \n", EN_id(v[i]), xyz[0], xyz[1], xyz[2]);
  }
  fprintf(fp, "=======================================\n");
  fprintf(fp, "Number of Mesh Edges = %d\n", e.size());
  for (int i = 0; i < e.size(); i++)
  {
    pVertex v0 = E_vertex(e[i], 0);
    pVertex v1 = E_vertex(e[i], 1);
    int v_0 = EN_id(v0);
    int v_1 = EN_id(v1);
    fprintf(fp, "%d %d %d \n", EN_id(e[i]), v_0, v_1);
  }
  fprintf(fp, "=======================================\n");
  fprintf(fp, "Number of Mesh Faces = %d\n", f.size());
  for (int i = 0; i < f.size(); i++)
  {
    pPList vOnF = F_vertices(f[i], 1);
    pVertex v0 = static_cast<pVertex>(PList_item(vOnF, 0));
    pVertex v1 = static_cast<pVertex>(PList_item(vOnF, 1));
    pVertex v2 = static_cast<pVertex>(PList_item(vOnF, 2));
    int v_0 = EN_id(v0);
    int v_1 = EN_id(v1);
    int v_2 = EN_id(v2);
    fprintf(fp, "%d %d %d %d \n", EN_id(f[i]), v_0, v_1, v_2);
  }
  fprintf(fp, "=======================================\n");
  fprintf(fp, "Number of Mesh Regions = %d\n", r.size());
  
  fclose(fp);
}
