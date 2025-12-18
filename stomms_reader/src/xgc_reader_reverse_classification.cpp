#include "xgc_reader_reverse_classification.hpp"

/******************************/
// Class ReverseClassification
/******************************/
ReverseClassification::ReverseClassification(const Omega_h::Mesh& mesh, const Model& model, 
                                             const MeshClassification& meshClass, 
                                             const ModelTopology& modelTopo)
{
  omegahMesh = mesh;
  geomModel = model;
  meshClassification = meshClass;
  modelTopology = modelTopo;

  setVerticesReverseClassification();
  setEdgesReverseClassification();
  setfacesReverseClassification();
  setRemainingModelEntities();

}

// Function to set reverse classification from classification 
// information of mesh vertices.
void ReverseClassification::setVerticesReverseClassification()
{
  auto vertId = omegahMesh.globals(Omega_h::VERT);
  for (int i = 0; i < vertId.size(); i++)
  {
    int id = vertId[i];
    TopoType t = meshClassification.getMeshVertexClassDim(id);
    GeomIdType geomId = meshClassification.getMeshVertexClassId(id);
    if (t == TopoType::Vertex)
      vertexToVertexReverseClassification[geomId].push_back(id);
    else if (t == TopoType::Edge)
      edgeToVertexReverseClassification[geomId].push_back(id);
    else if (t == TopoType::Face)
      faceToVertexReverseClassification[geomId].push_back(id);
  }
}

// Function to set reverse classification from classification 
// information of mesh edges.
void ReverseClassification::setEdgesReverseClassification()
{
  auto edgeId = omegahMesh.globals(Omega_h::EDGE);
  for (int i = 0; i < edgeId.size(); i++)
  {
    int id = edgeId[i];
    TopoType t = meshClassification.getMeshEdgeClassDim(id);
    GeomIdType geomId = meshClassification.getMeshEdgeClassId(id);
    if (t == TopoType::Edge)
      edgeToEdgeReverseClassification[geomId].push_back(id);
    else if (t == TopoType::Face)
      faceToEdgeReverseClassification[geomId].push_back(id);
  }
}

// Function to set reverse classification from classification 
// information of mesh faces.
void ReverseClassification::setfacesReverseClassification()
{
  auto faceId = omegahMesh.globals(Omega_h::FACE);
  for (int i = 0; i < faceId.size(); i++)
  {
    int id = faceId[i];
    TopoType t = meshClassification.getMeshFaceClassDim(id);
    GeomIdType geomId = meshClassification.getMeshFaceClassId(id);
    if (t == TopoType::Face)
      faceToFaceReverseClassification[geomId].push_back(id);
  }
}

// Since we are using classification information to construct reverse classification, we 
// only got model entities that have a mesh entity classified on them and missed remaining 
// model entities (for example if a model entity doesn't have a mesh vertex on it, it was 
// skipped).Now if we make a query mesh vertices on such model face it will cause an error. 
// Make sure to set them (empty vector) also for query purposes and consistency.
void ReverseClassification::setRemainingModelEntities()
{
  std::vector<MeshIdType> vectorToSet = {};  // Empty vector just for consistency

  // Step 1: First set edges to vertices map
  std::map <EdgeIdType, ModelEdge> modelEdges = geomModel.getModelEdges();
  for (const auto& edge: modelEdges)
  {
    EdgeIdType edgeId = edge.first;
    if (edgeToVertexReverseClassification.find(edgeId) == edgeToVertexReverseClassification.end())
      edgeToVertexReverseClassification[edgeId] = vectorToSet;
  }

  // Step 2: Now set faces to edges and faces to vertices
  std::map <SurfaceIdType, ModelFace> modelFaces = geomModel.getModelFaces();
  for (const auto& face: modelFaces)
  {
    SurfaceIdType faceId = face.first;
    if (faceToEdgeReverseClassification.find(faceId) == faceToEdgeReverseClassification.end())
      faceToEdgeReverseClassification[faceId] = vectorToSet;

    if (faceToVertexReverseClassification.find(faceId) == faceToVertexReverseClassification.end())
      faceToVertexReverseClassification[faceId] = vectorToSet;
  }

}

// Given geometric id of the model vertex, return the mesh vertex classified on it.
std::vector <MeshIdType> ReverseClassification::getVertexToVertexReverseClassification(const GeomIdType& gId) const
{
  std::vector <MeshIdType> v2v;
  if (vertexToVertexReverseClassification.find(gId) != vertexToVertexReverseClassification.end())
    v2v = vertexToVertexReverseClassification.at(gId);
  else
  {
    std::cout << "Error: Incorrect Model Vertex Id for Reverse Classification\n";
    std::cout << "Error: Model Vertex Id = " << gId << " does not exist\n";
    exit(1);
  }

  return v2v;
}

// Given geometric id of the model edge, return the vector of mesh edges classified on it.
std::vector <MeshIdType> ReverseClassification::getEdgeToEdgeReverseClassification(const GeomIdType& gId) const
{
  std::vector <MeshIdType> e2e;
  if (edgeToEdgeReverseClassification.find(gId) != edgeToEdgeReverseClassification.end())
    e2e = edgeToEdgeReverseClassification.at(gId);
  else
  {
    std::cout << "Error: Incorrect Model Edge Id for Reverse Classification\n";
    std::cout << "Error: Model Edge Id = " << gId << " does not exist\n";
    exit(1);
  }

  return e2e;
}

// Given geometric id of the model face, return the vector of mesh faces classified on it.
std::vector <MeshIdType> ReverseClassification::getFaceToFaceReverseClassification(const GeomIdType& gId) const
{
  std::vector <MeshIdType> f2f;
  if (faceToFaceReverseClassification.find(gId) != faceToFaceReverseClassification.end())
    f2f = faceToFaceReverseClassification.at(gId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id for Reverse Classification\n";
    std::cout << "Error: Model Face Id = " << gId << " does not exist\n";
    exit(1);
  }

  return f2f;
}

// Given geometric id of the model edge, return the vector of mesh vertices classified on it.
std::vector <MeshIdType> ReverseClassification::getEdgeToVertexReverseClassification(const GeomIdType& gId, bool includeClosure) const
{
  std::vector <MeshIdType> e2v;

  // Step 1: First get the mesh vertices classified on the model edge.
  if (edgeToVertexReverseClassification.find(gId) != edgeToVertexReverseClassification.end())
    e2v = edgeToVertexReverseClassification.at(gId);
  else
  {
    std::cout << "Error: Incorrect Model Edge Id for Reverse Classification\n";
    std::cout << "Error: Model Edge Id = " << gId << " does not exist\n";
    exit(1);
  }

  // Step 2: Get the mesh vertices classified on the closure of the model edge (i.e on model vertices).
  if (includeClosure)
  {
     std::vector <VertexIdType> vertexIds = modelTopology.getAdjacentVerticesOnEdge(gId);
     std::vector <MeshIdType> v2v;
     for (int i = 0; i < vertexIds.size(); i++)
     {
       v2v = getVertexToVertexReverseClassification(vertexIds[i]);
       e2v.insert(e2v.end(), v2v.begin(), v2v.end());
     } 
  }

  return e2v;
}

// Given geometric id of the model face, return the vector of mesh vertices classified on it.
std::vector <MeshIdType> ReverseClassification::getFaceToVertexReverseClassification(const GeomIdType& gId, bool includeClosure) const
{
  std::vector <MeshIdType> f2v;

  // Step 1: First get the mesh vertices classified on the model face.
  if (faceToVertexReverseClassification.find(gId) != faceToVertexReverseClassification.end())
    f2v = faceToVertexReverseClassification.at(gId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id for Reverse Classification\n";
    std::cout << "Error: Model Face Id = " << gId << " does not exist\n";
    exit(1);
  }

  // Step 2: Get the mesh vertices classified on the closure of the model face (i.e on model edges, vertices).
  if (includeClosure)
  {
    std::vector <VertexIdType> edgeIds = modelTopology.getAdjacentEdgesOnFace(gId);
    std::vector <MeshIdType> e2v;
    for (int i = 0; i < edgeIds.size(); i++)
    {
      e2v = getEdgeToVertexReverseClassification(edgeIds[i], includeClosure);
      f2v.insert(f2v.end(), e2v.begin(), e2v.end());
    }
  }

  return f2v;
}

// Given geometric id of the model face, return the vector of mesh edges classified on it.
std::vector <MeshIdType> ReverseClassification::getFaceToEdgeReverseClassification(const GeomIdType& gId, bool includeClosure) const
{
  std::vector <MeshIdType> f2e;

  // Step 1: First get the mesh edges classified on the model face.
  if (faceToEdgeReverseClassification.find(gId) != faceToEdgeReverseClassification.end())
    f2e = faceToEdgeReverseClassification.at(gId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id for Reverse Classification\n";
    std::cout << "Error: Model Face Id = " << gId << " does not exist\n";
    exit(1);
  }

  // Step 2: Get the mesh edges classified on the closure of the model face (i.e on model edges).
  if (includeClosure)
  {
    std::vector <EdgeIdType> edgeIds = modelTopology.getAdjacentEdgesOnFace(gId);
    std::vector <MeshIdType> e2e;
    for (int i = 0; i < edgeIds.size(); i++)
    {
      e2e = getEdgeToEdgeReverseClassification(edgeIds[i]);
      f2e.insert(f2e.end(), e2e.begin(), e2e.end());
    }
  }

  return f2e;
}

