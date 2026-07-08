#include "xgc_reader_interface.hpp"

/****************************/
// Class XgcMesh
/****************************/
XgcMesh::XgcMesh(std::string adiosFileName):adiosFile(adiosFileName)
{
  // Step 1: Create adios2 object and io.
  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("xgcReader");

  // Step 2: Start the reader engine
  adios2::Engine reader = io.Open(adiosFile, adios2::Mode::Read);
  reader.BeginStep();

  // Step 3: Get the mesh name & number of planes
  meshName = setMeshName(io);
  numPlanes = setNumPlanes(io);

  // Step 4: Read Omegah meshes on the planes. Also,
  // set Omegah Meshes to plane meshes
  omegahMeshes = setOmegahMeshes();
  planeMeshes = setPlaneMeshes(io, reader);
}

// Function to read mesh name and set it in the class.
std::string XgcMesh::setMeshName(adios2::IO &io) 
{
  std::string name;  // return mesh

  // Step 1: Set the path of the groups to read.
  auto g = io.InquireGroup('/');  // group identifier
  std::string groupName = "";  // the very first group in the list
  
  // Step 2: Read groups and make sure the size of groups is 1 (only mesh name).
  // If greater than 1, throw an error.
  auto groups = g.AvailableGroups();
  if (groups.size() == 1)
    name = groups[0];
  else if (groups.size() > 1)
  {
   std::cout << "More than one mesh names exist in the file\n";
   std::cout << "Make sure only one mesh is provided in the adios2 file\n";
   std::cout << "========== List of mesh names in file is provided below ===========\n";
   for (const auto &g : groups)
    std::cout << "Group name: " << g << "\n";  
  
   exit(1);
  }

  return name; 
}

// Function to get mesh name from XgcMesh class.
std::string XgcMesh::getMeshName()
{
  return meshName;
}


// Function to read number of planes and set them in the reader class.
int XgcMesh::setNumPlanes(adios2::IO &io)
{
  // Step 1: Set the path of the groups to read.
  auto g = io.InquireGroup('/');  // group identifier
  std::string groupName = meshName + "/planes";  // group followed by mesh name
  g.setPath(groupName);  

  // Step 2: Read groups (number of planes).
  auto groups = g.AvailableGroups();
  int nPlanes = groups.size();
  
  return nPlanes;
}

// Function to return number of planes in a mesh.
int XgcMesh::getNumPlanes()
{
  return numPlanes;
}

// Function to set a vector of Omega_h meshes (one for each plane).
std::vector <Omega_h::Mesh> XgcMesh::setOmegahMeshes()
{
  // Step 1: Initialize Omegah objects
  auto lib = Omega_h::Library(NULL, NULL, MPI_COMM_SELF);
  auto world = lib.world();
 
  Omega_h::filesystem::path adiosOut = adiosFile; 
  std::vector <Omega_h::Mesh> meshes;
  for (int i = 0; i < numPlanes; i++)
  {
    std::string name =  meshName + "/planes/" + std::to_string(i)+"/";
    Omega_h::Mesh planeMesh = Omega_h::adios::read(adiosOut, &lib, name);  
    meshes.push_back(planeMesh);

  }

  return meshes;
}

// Function to set a vector of plane meshes.
std::vector <PlaneMesh> XgcMesh::setPlaneMeshes(adios2::IO &io, adios2::Engine &reader)
{
  std::vector <PlaneMesh> meshesVector;
  for (int i = 0; i < omegahMeshes.size(); i++)
  {
    Omega_h::Mesh oMesh = omegahMeshes[i];
    PlaneMesh pMesh(oMesh, io, reader, meshName, i);
    meshesVector.push_back(pMesh);
  }
  return meshesVector;
}

// Function to return vector of plane meshes for internal use.
std::vector <PlaneMesh> XgcMesh::getPlaneMeshes()
{
  return planeMeshes;
}

// Given the plane number, return PlaneMesh for that particular plane. 
const PlaneMesh& XgcMesh::getMeshAtPlane(const int& planeNum)
{

  isPlaneValid(planeNum);
  return planeMeshes[planeNum];
}

// Given the plane number, get Omegah mesh at that plane.
Omega_h::Mesh XgcMesh::getOmegahMeshatPlane(const int& planeNum)
{
  isPlaneValid(planeNum);
  const PlaneMesh& planeMesh = getMeshAtPlane(planeNum);
  Omega_h::Mesh omegahMesh = planeMesh.getOmegahMesh();
  return omegahMesh;
}

// Function to check the validity of the plane when queries are made.
bool XgcMesh::isPlaneValid(const int& planeNum)
{
  if (planeNum >= getNumPlanes())
  {
    std::cout << "Max plane index allowed for this case = " << numPlanes-1 << "\n";
    std::cout << "Make sure input plane number index is lower than this number " << "\n";
    std::cout << "Error: Current input index is " << planeNum << "\n";
    exit(1);
  }
  
  return true;
}

// Given the type of physics region, return the number of flux curves on it.
int XgcMesh::getNumCurvesAtRegion(int physicsRegion, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);

  std::vector <ModelCurve> fluxCurves = pMesh.getCurvesAtPhysicsRegion(physicsRegion);

  return fluxCurves.size();
}

// Given the type of physics region, return the the indices of flux curves on it.
std::vector <CurveIdType> XgcMesh::getCurvesAtRegion (int physicsRegion, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  std::vector <ModelCurve> fluxCurves = pMesh.getCurvesAtPhysicsRegion(physicsRegion);

  std::vector <CurveIdType> curveIds;
  for (int i = 0; i < fluxCurves.size(); i++)
    curveIds.push_back(fluxCurves[i].getCurveId());

  return curveIds;
}

// Given the required point type and plane #, return x-points or o-points on the plane.
std::vector <VertexIdType> XgcMesh::getCriticalPoints(PointType pType, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);

  std::vector <VertexIdType> points;
  if (pType == PointType::XPoint)
    points = pMesh.getModel().getXpoints();
  else if (pType == PointType::OPoint)
    points = pMesh.getModel().getOpoints();
   
  return points;
}

// Given the id of a surface and plane #, return physics type of the surface.
SurfaceType XgcMesh::getPhysicsRegionType(SurfaceIdType surfaceId, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  std::map <SurfaceIdType, ModelFace> modelFacesAtPlane = pMesh.getModel().getModelFaces();
  ModelFace face;

  if (modelFacesAtPlane.find(surfaceId) != modelFacesAtPlane.end())
    face = modelFacesAtPlane.at(surfaceId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id\n";
    std::cout << "Error: Model Face Id = " << surfaceId << " does not exist\n";
    exit(1);
  }
  
  SurfaceType sType = face.getSurfaceType();
  return sType;
}

// Given the id of a model curve and Plane #, return curve type.
CurveType XgcMesh::getCurveType(CurveIdType curveId, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);

  std::map <CurveIdType, ModelCurve> modelCurvesAtPlane = pMesh.getModel().getModelCurves();
  ModelCurve curve;
  if (modelCurvesAtPlane.find(curveId) != modelCurvesAtPlane.end())
    curve = modelCurvesAtPlane.at(curveId);
  else
  {
    std::cout << "Error: Incorrect Model Curve Id\n";
    std::cout << "Error: Model Curve Id = " << curveId << " does not exist\n";
    exit(1);
  }

  CurveType cType = curve.getCurveType();
  return cType;
}

// Given the curveType (closed,open etc.) and plane #, return all flux curves of that type.
std::vector <CurveIdType> XgcMesh::getCurvesForType(CurveType cType, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);

  std::vector <CurveIdType> curves = pMesh.getModel().getModelCurvesFromType(cType);
  return curves;
}

// Given the model edge id and plane #, return the curve id on which model edge is 
// classified on.
CurveIdType XgcMesh::getCurveIdFromGeometricEdge(EdgeIdType edgeId, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);

  CurveIdType curveId = pMesh.getModel().getModelCurveFromEdgeId(edgeId);
  return curveId;
}

// Given the id of a model curve, and plane #, this function returns all the geometric
// edges on that model curve.
std::vector<EdgeIdType> XgcMesh::getCurveGeometricEdges(CurveIdType curveId, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  
  std::map <CurveIdType, ModelCurve> modelCurvesAtPlane = pMesh.getModel().getModelCurves();
  ModelCurve curve;
  if (modelCurvesAtPlane.find(curveId) != modelCurvesAtPlane.end())
    curve = modelCurvesAtPlane.at(curveId);
  else
  {
    std::cout << "Error: Incorrect Model Curve Id\n";
    std::cout << "Error: Model Curve Id = " << curveId << " does not exist\n";
    exit(1);
  }
  
  std::vector <EdgeIdType> edgesOnCurve = curve.getModelEdgesGeomIds();
  return edgesOnCurve;
}

// Function to return model topology of the model entity on which mesh entity is classified. 
// This will return the dimension of the model entity on which mesh entity is classified.
TopoType XgcMesh::getGeometricTopology(TopoType topoType, MeshIdType id, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  const MeshClassification& mClass = pMesh.getMeshClassification();

  TopoType t;  // Model Topology
  if (topoType == TopoType::Vertex)  // Mesh vertex
    t =  mClass.getMeshVertexClassDim(id);
  else if (topoType == TopoType::Edge)  // Mesh Edge
    t = mClass.getMeshEdgeClassDim(id);
  else if (topoType == TopoType::Face)  // Mesh Face
    t = mClass.getMeshFaceClassDim(id);

  return t;
}

// Function to return id of the model entity on which mesh entity is classified. This function 
// must be used with the getGeometricTopology to get complete classification information.
GeomIdType XgcMesh::getGeometricClassification(TopoType topoType, MeshIdType id, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  const MeshClassification& mClass = pMesh.getMeshClassification();

  GeomIdType geomId;  // Classification Id
  if (topoType == TopoType::Vertex)  // Mesh vertex
    geomId =  mClass.getMeshVertexClassId(id);
  else if (topoType == TopoType::Edge)  // Mesh Edge
    geomId = mClass.getMeshEdgeClassId(id);
  else if (topoType == TopoType::Face)  // Mesh Face
    geomId = mClass.getMeshFaceClassId(id);

  return geomId;
  
}

// Reverse Classification.
// Given the geometric model id and topology, return the reverse classification for the desired mesh topology.
std::vector<MeshIdType> XgcMesh::getMeshEntities(TopoType geomTopoType, GeomIdType geomId, TopoType meshTopoType, bool includeClosure, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  const ReverseClassification& rClass = pMesh.getReverseClassification();

  std::vector <MeshIdType> meshEntIdsOnGeomEnt;
  if (geomTopoType == TopoType::Vertex && meshTopoType == TopoType::Vertex)
    meshEntIdsOnGeomEnt = rClass.getVertexToVertexReverseClassification(geomId);
  else if (geomTopoType == TopoType::Edge && meshTopoType == TopoType::Vertex)
    meshEntIdsOnGeomEnt = rClass.getEdgeToVertexReverseClassification(geomId, includeClosure);
  else if (geomTopoType == TopoType::Face && meshTopoType == TopoType::Vertex)
    meshEntIdsOnGeomEnt = rClass.getFaceToVertexReverseClassification(geomId, includeClosure);
  else if (geomTopoType == TopoType::Edge && meshTopoType == TopoType::Edge)
    meshEntIdsOnGeomEnt = rClass.getEdgeToEdgeReverseClassification(geomId);
  else if (geomTopoType == TopoType::Face && meshTopoType == TopoType::Edge)
    meshEntIdsOnGeomEnt = rClass.getFaceToEdgeReverseClassification(geomId, includeClosure);
  else if (geomTopoType == TopoType::Face && meshTopoType == TopoType::Face)
    meshEntIdsOnGeomEnt = rClass.getFaceToFaceReverseClassification(geomId);

  return meshEntIdsOnGeomEnt;
}

// Given the model entity topology, and its geometric id, returns all adjacents entities of dimension geomTopoOut.
std::vector <GeomIdType> XgcMesh::getModelAdjEnts(TopoType geomTopoIn, GeomIdType geomEntId, TopoType geomTopoOut, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  const ModelTopology& modelTopo = pMesh.getModelTopology();

  std::vector <GeomIdType> adjEnts;
  if (geomTopoIn == TopoType::Vertex && geomTopoOut == TopoType::Edge)
    adjEnts = modelTopo.getAdjacentEdgesOnVertex(geomEntId);
  else if (geomTopoIn == TopoType::Vertex && geomTopoOut == TopoType::Face)
    adjEnts = modelTopo.getAdjacentFacesOnVertex(geomEntId);
  else if (geomTopoIn == TopoType::Edge && geomTopoOut == TopoType::Vertex)
    adjEnts = modelTopo.getAdjacentVerticesOnEdge(geomEntId);
  else if (geomTopoIn == TopoType::Edge && geomTopoOut == TopoType::Face)
    adjEnts = modelTopo.getAdjacentFacesOnEdge(geomEntId);
  else if (geomTopoIn == TopoType::Face && geomTopoOut == TopoType::Vertex)
    adjEnts = modelTopo.getAdjacentVerticesOnFace(geomEntId);
  else if (geomTopoIn == TopoType::Face && geomTopoOut == TopoType::Edge)
    adjEnts = modelTopo.getAdjacentEdgesOnFace(geomEntId);
  else
  {
    int inDim = static_cast<int> (geomTopoIn);
    int outDim = static_cast<int> (geomTopoOut);
    if (inDim < 0 || inDim > 2)
    {
      std::cout << "Error: Input dimension of " << inDim << " is not valid. Make sure its 0,1, or 2\n";
      exit(1);
    }
    if (outDim < 0 || outDim > 2)
    {
      std::cout << "Error: Output dimension of " << outDim << " is not valid. Make sure its 0,1, or 2\n";
      exit(1);
    }
  }

  return adjEnts;
} 

// Function to take an index of a private region and returns a vector of the adjacent x-points.
std::vector<VertexIdType> XgcMesh::getAdjacentXpoints(SurfaceIdType privateFaceIndex, PlaneId planeId)
{
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  std::map<SurfaceIdType, ModelFace> modelFaces = pMesh.getModel().getModelFaces();

  if (modelFaces.find(privateFaceIndex) == modelFaces.end())
  {
    std::cout << "Error: Incorrect Surface Id for the adjacent X-point query\n";
    std::cout << "Error: Surface Id = " << privateFaceIndex << " does not exist\n";
    exit(1);
  }

  ModelFace f = modelFaces.at(privateFaceIndex);
  std::cout << "Surface Type = " << static_cast<int>(f.getSurfaceType()) << "\n";
  if (f.getSurfaceType() != SurfaceType::Private)
  {
    std::cout << "Error: Given Surface = " << privateFaceIndex << " is not a private region\n";
    exit(1);
  }
 
  // Get the private physics region index on which face is classified.
  int privateRegionIndx = pMesh.getModel().getPrivateRegionIndex(privateFaceIndex);
  std::map<int, std::vector<ModelFace>> pvtRegions = pMesh.getModel().getPrivateRegions();
  std::vector <ModelFace> privateModelFaces = pvtRegions.at(privateRegionIndx);
  std::vector <int> xPts;
  for (int i = 0; i < privateModelFaces.size(); i++)
  {
    int fId = privateModelFaces[i].getGeomId();
    std::vector <GeomIdType> vertices = getModelAdjEnts(TopoType::Face, fId, TopoType::Vertex, planeId);
    for (int j = 0; j < vertices.size(); j++)
    {
      VertexIdType vId = vertices[j];
      ModelVertex v = pMesh.getModel().getModelVertexFromId(vId);
      if (v.getPointType() == PointType::XPoint)
        xPts.push_back(vId);
    }
  }

  return xPts; 
}

// Given the mesh entity (vertex or face) return the physics regions its classified on.
SurfaceType XgcMesh::getPhysicsRegionForMeshEnt(TopoType meshTopoType, MeshIdType id, PlaneId planeId)
{
  isPlaneValid(planeId);
  int dim = static_cast<int>(meshTopoType);
  if (dim != 0 && dim != 2)
  {
    std::cout << "Error: Input dimension of " << dim  << " is not valid. Make sure its 0, or 2\n";
    std::cout << "Supported mesh entities: mesh vertices (dim == 0) and mesh faces (dim == 2)\n";
    exit(1);
  }
  
  TopoType geomTopoType = getGeometricTopology(meshTopoType, id, planeId);
  GeomIdType geomEntId = getGeometricClassification(meshTopoType, id, planeId);

  SurfaceType physicsRegion;
  if (geomTopoType == TopoType::Face)
    physicsRegion = getPhysicsRegionType(geomEntId, planeId);
  else
  {
    std::cout << "Mesh entity is either classified on model edge or vertex\n";
    exit(1);
  }

  return physicsRegion; 
}

// Given a surface (model face), return all non-aligned mesh vertices on it.
std::map<MeshIdType, std::vector <CurveIdType>> XgcMesh::getNonAlignedMeshVerticesOnModelSurface(SurfaceIdType surfaceId, PlaneId planeId)
{
  // Step 1: Check the plane validity, and get planer mesh.
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  const NonAlignedVertices& nonAlignedVertices = pMesh.getNonAlignedVertices();

  // Step 2: Get the non-aligned vertices vector on the face from the maps.
  std::vector <NonAlignedVertex> verticesOnFace= nonAlignedVertices.getNonAlignedVerticesOnFace(surfaceId);
  std::map <MeshIdType, std::vector <CurveIdType>> verticesOnF;
  for (int i = 0; i < verticesOnFace.size(); i++)
  {
    const NonAlignedVertex& v = verticesOnFace[i];
    MeshIdType vId = v.getVertex();
    std::vector <CurveIdType> boundingCurves = v.getBoundingFluxCurves();
    verticesOnF[vId] = boundingCurves;
  }

  return verticesOnF;
}


// Given just PlaneId, return all non-aligned mesh vertices on it.
std::map<MeshIdType, std::vector <CurveIdType>> XgcMesh::getNonAlignedMeshVerticesOnPlane(PlaneId planeId)
{
  // Step 1: Check the plane validity, and get planer mesh.
  isPlaneValid(planeId);
  const PlaneMesh& pMesh = getMeshAtPlane(planeId);
  const NonAlignedVertices& nonAlignedVertices = pMesh.getNonAlignedVertices();

  // Step 2: Get the non-aligned vertices vector for each face.
  std::map <SurfaceIdType, std::vector <NonAlignedVertex>> nonAlignedVerticesMap;
  nonAlignedVerticesMap = nonAlignedVertices.getNonAlignedVerticesOnModel();
  
  // Step 3: Iterate over each face and get the corresponding vector of non-
  // aligned vertices on it. 
  std::map <MeshIdType, std::vector <CurveIdType>> verticesOnModel;
  for (const auto& face: nonAlignedVerticesMap)
  {
    SurfaceIdType faceId = face.first;
    std::vector <NonAlignedVertex> verticesOnF = face.second;

    // Step 3.1: Iterate over the vertices in vector and save them in verticesOnModel.
    for (int i = 0; i < verticesOnF.size(); i++) 
    {
      NonAlignedVertex v = verticesOnF[i];
      MeshIdType vId = v.getVertex();
      std::vector <CurveIdType> boundingCurves = v.getBoundingFluxCurves();
      verticesOnModel[vId] = boundingCurves;
    }
  }

  return verticesOnModel;
}

/*******************************/
// Class XgcBackgroundGridData
/*******************************/
XgcBackgroundGridData::XgcBackgroundGridData(std::string adiosFileName):adiosFile(adiosFileName)
{
  // Step 1: Create adios2 object and io.
  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("xgcReader");

  // Step 2: Start the reader engine
  adios2::Engine reader = io.Open(adiosFile, adios2::Mode::Read);
  reader.BeginStep();

  // Step 3: Verify grid data exists in adios2 file.
  gridDataName = setGridDataName(io);

  // Step 4: Set grid data from adios2 file to class EqdskGridData
  eqdskGridData = EqdskGridData(io, reader, gridDataName);
}

std::string XgcBackgroundGridData::setGridDataName(adios2::IO &io)
{
  std::string name;  // return grid data name

  // Step 1: Set the path of the groups to read.
  auto g = io.InquireGroup('/');  // group identifier
  std::string groupName = "";  // the very first group in the list
  
  // Step 2: Read groups and make sure the size of groups is 1 (only top level mesh name).
  // If greater than 1, throw an error.
  auto groups = g.AvailableGroups();
  if (groups.size() == 1)
    name = groups[0];
  else if (groups.size() > 1)
  {
   std::cout << "More than one mesh names exist in the file\n";
   std::cout << "Make sure only one mesh is provided in the adios2 file\n";
   std::cout << "========== List of mesh names in file is provided below ===========\n";
   for (const auto &g : groups)
    std::cout << "Group name: " << g << "\n";  
  
   exit(1);
  }

  // Step 3: Make sure grid field data exist in the file.
  g.setPath(name);
  groups = g.AvailableGroups();
  bool gridFound = false;
  for (const auto &g : groups)
  {
    if (g == "fieldGridData")
    {
      std::cout << "Background field data is found in the adios2 file\n";
      name = name + "/" + g;
      gridFound = true;
      break;
    }
  }

  // Step 4: Exit the program with an error message if field data is not found.
  if (!gridFound)
  {
    std::cout << "The variable = " << name << "/fieldGridData does not exist in the adios2 file\n";
    std::cout << "Make sure to use correct adios2 file with valid fieldgridData before using XgcBackgroundGridData\n"; 
    exit(1);
  }
  return name; 
}
