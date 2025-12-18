#include "xgc_reader_topology.hpp"

/****************************/
// Model Topology
/****************************/
ModelTopology::ModelTopology(const adios2::IO& io, const adios2::Engine& reader, std::string name, int planeNumber)
             :ioPlane(io), readerPlane(reader), meshName(name), planeNum(planeNumber)
{
  // Step 1: Find the groups for the particular planes
  auto g = ioPlane.InquireGroup('/');  // group identifier
  std::string groupName = meshName + "/planes/" + std::to_string(planeNumber) + "/modelAdj";
  g.setPath(groupName);

  // Step 2: Read groups (Physics classification on model entities).
  auto groups = g.AvailableGroups();
  for (const auto &group : groups)
  {
    std::string name = groupName + "/" + group;
    if (std::stoi(group) == 0)
      setVertexAdjacencies(name); 
    if (std::stoi(group) == 1) 
      setEdgeAdjacencies(name);
    if (std::stoi(group) == 2) 
      setFaceAdjacencies(name); 
  }
}

// Function to set the model adjacencies on model vertices.
void ModelTopology::setVertexAdjacencies(std::string groupName)
{
  std::vector <int32_t> vIds;
  std::vector <int32_t> eAdjRange, eAdjData;
  std::vector <int32_t> fAdjRange, fAdjData;  

  // Step 1: Find the variables for vertex adjacencies
  std::vector <std::string> variables = findVarInGroup(ioPlane, groupName);
  assert(variables.size() == 5);  // Just need to verify we have all the variables

  // Step 2: Read groups (vertexIds, data, range)
  std::string varName = groupName + "/vertexIds";
  readAdios2Array(ioPlane, readerPlane, vIds, varName);
  varName = groupName + "/toEdge/range";
  readAdios2Array(ioPlane, readerPlane, eAdjRange, varName);
  varName = groupName + "/toEdge/data";
  readAdios2Array(ioPlane, readerPlane, eAdjData, varName);
  varName = groupName + "/toFace/range";
  readAdios2Array(ioPlane, readerPlane, fAdjRange, varName);
  varName = groupName + "/toFace/data";
  readAdios2Array(ioPlane, readerPlane, fAdjData, varName);

  // Check the data sizes to make sure they are correct and as expected.
  assert(vIds.size() == eAdjRange.size() - 1 && vIds.size() == fAdjRange.size() - 1);

  // Step 3: Read and store the adjacency data on each vertex.
  for (int i = 0; i < vIds.size(); i++)
  {
    VertexIdType v = vIds[i];
    std::vector <EdgeIdType> edgesOnV;
    std::vector <SurfaceIdType> facesOnV;
    for (int j = eAdjRange[i]; j < eAdjRange[i+1]; j++)
      edgesOnV.push_back(eAdjData[j]);
    for (int j = fAdjRange[i]; j < fAdjRange[i+1]; j++)
      facesOnV.push_back(fAdjData[j]);
 
    vertexToEdge[v] = edgesOnV;
    vertexToFace[v] = facesOnV;
  }
}

// Function to set the model adjacencies on model edges.
void ModelTopology::setEdgeAdjacencies(std::string groupName)
{
  std::vector <int32_t> eIds;
  std::vector <int32_t> vAdjRange, vAdjData;
  std::vector <int32_t> fAdjRange, fAdjData;  

  // Step 1: Find the variables for edge adjacencies
  std::vector <std::string> variables = findVarInGroup(ioPlane, groupName);
  assert(variables.size() == 5);  // Just need to verify we have all the variables

  // Step 2: Read groups (edgeIds, data, range)
  std::string varName = groupName + "/edgeIds";
  readAdios2Array(ioPlane, readerPlane, eIds, varName);
  varName = groupName + "/toVertex/range";
  readAdios2Array(ioPlane, readerPlane, vAdjRange, varName);
  varName = groupName + "/toVertex/data";
  readAdios2Array(ioPlane, readerPlane, vAdjData, varName);
  varName = groupName + "/toFace/range";
  readAdios2Array(ioPlane, readerPlane, fAdjRange, varName);
  varName = groupName + "/toFace/data";
  readAdios2Array(ioPlane, readerPlane, fAdjData, varName);

  // Check the data sizes to make sure they are correct and as expected.
  assert(eIds.size() == vAdjRange.size() - 1 && eIds.size() == fAdjRange.size() - 1);

  // Step 3: Read and store the adjacency data on each edge.
  for (int i = 0; i < eIds.size(); i++)
  {
    EdgeIdType e = eIds[i];
    std::vector <VertexIdType> verticesOnE;
    std::vector <SurfaceIdType> facesOnE;
    for (int j = vAdjRange[i]; j < vAdjRange[i+1]; j++)
      verticesOnE.push_back(vAdjData[j]);
    for (int j = fAdjRange[i]; j < fAdjRange[i+1]; j++)
      facesOnE.push_back(fAdjData[j]);
 
    edgeToVertex[e] = verticesOnE;
    edgeToFace[e] = facesOnE;
  }
}

// Function to set the model adjacencies on model faces.
void ModelTopology::setFaceAdjacencies(std::string groupName)
{
  std::vector <int32_t> fIds;
  std::vector <int32_t> vAdjRange, vAdjData;
  std::vector <int32_t> eAdjRange, eAdjData;  

  // Step 1: Find the variables for face adjacencies
  std::vector <std::string> variables = findVarInGroup(ioPlane, groupName);
  assert(variables.size() == 5);  // Just need to verify we have all the variables

  // Step 2: Read groups (faceIds, data, range)
  std::string varName = groupName + "/faceIds";
  readAdios2Array(ioPlane, readerPlane, fIds, varName);
  varName = groupName + "/toVertex/range";
  readAdios2Array(ioPlane, readerPlane, vAdjRange, varName);
  varName = groupName + "/toVertex/data";
  readAdios2Array(ioPlane, readerPlane, vAdjData, varName);
  varName = groupName + "/toEdge/range";
  readAdios2Array(ioPlane, readerPlane, eAdjRange, varName);
  varName = groupName + "/toEdge/data";
  readAdios2Array(ioPlane, readerPlane, eAdjData, varName);

  // Check the data sizes to make sure they are correct and as expected.
  assert(fIds.size() == vAdjRange.size() - 1 && fIds.size() == eAdjRange.size() - 1);

  // Step 3: Read and store the adjacency data on each face.
  for (int i = 0; i < fIds.size(); i++)
  {
    SurfaceIdType f = fIds[i];
    std::vector <VertexIdType> verticesOnF;
    std::vector <EdgeIdType> edgesOnF;
    for (int j = vAdjRange[i]; j < vAdjRange[i+1]; j++)
      verticesOnF.push_back(vAdjData[j]);
    for (int j = eAdjRange[i]; j < eAdjRange[i+1]; j++)
      edgesOnF.push_back(eAdjData[j]);
 
    faceToVertex[f] = verticesOnF;
    faceToEdge[f] = edgesOnF;
  }
}

// Given geometric id of the model vertex, returns the model edges adjacent it.
std::vector <EdgeIdType> ModelTopology::getAdjacentEdgesOnVertex(const VertexIdType& vId) const
{
  std::vector <EdgeIdType> edgesOnV;
  if (vertexToEdge.find(vId) != vertexToEdge.end())
    edgesOnV = vertexToEdge.at(vId);
  else
  {
    std::cout << "Error: Incorrect Model Vertex Id\n";
    std::cout << "Error: Model Vertex Id = " << vId << " does not exist\n";
    exit(1);
  }

  return edgesOnV;
}

// Given geometric id of the model vertex, returns the model faces adjacent it.
std::vector <SurfaceIdType> ModelTopology::getAdjacentFacesOnVertex(const VertexIdType& vId) const
{
  std::vector <SurfaceIdType> facesOnV;
  if (vertexToFace.find(vId) != vertexToFace.end())
    facesOnV = vertexToFace.at(vId);
  else
  {
    std::cout << "Error: Incorrect Model Vertex Id\n";
    std::cout << "Error: Model Vertex Id = " << vId << " does not exist\n";
    exit(1);
  }

  return facesOnV;
}

// Given geometric id of the model edge, returns the model vertices adjacent it.
std::vector <VertexIdType> ModelTopology::getAdjacentVerticesOnEdge(const EdgeIdType& eId) const
{
  std::vector <VertexIdType> verticesOnE;
  if (edgeToVertex.find(eId) != edgeToVertex.end())
    verticesOnE = edgeToVertex.at(eId);
  else
  {
    std::cout << "Error: Incorrect Model Edge Id\n";
    std::cout << "Error: Model Edge Id = " << eId << " does not exist\n";
    exit(1);
  }

  return verticesOnE;
}

// Given geometric id of the model edge, returns the model faces adjacent it.
std::vector <SurfaceIdType> ModelTopology::getAdjacentFacesOnEdge(const EdgeIdType& eId) const
{
  std::vector <SurfaceIdType> facesOnE;
  if (edgeToFace.find(eId) != edgeToFace.end())
    facesOnE = edgeToFace.at(eId);
  else
  {
    std::cout << "Error: Incorrect Model Edge Id\n";
    std::cout << "Error: Model Edge Id = " << eId << " does not exist\n";
    exit(1);
  }

  return facesOnE;
}

// Given geometric id of the model face, returns the model vertices adjacent it.
std::vector <VertexIdType> ModelTopology::getAdjacentVerticesOnFace(const SurfaceIdType& fId) const
{
  std::vector <VertexIdType> verticesOnF;
  if (faceToVertex.find(fId) != faceToVertex.end())
    verticesOnF = faceToVertex.at(fId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id\n";
    std::cout << "Error: Model Face Id = " << fId << " does not exist\n";
    exit(1);
  }
 
  return verticesOnF;
}

// Given geometric id of the model face, returns the model edges adjacent it.
std::vector <EdgeIdType> ModelTopology::getAdjacentEdgesOnFace(const SurfaceIdType& fId) const
{
  std::vector <EdgeIdType> edgesOnF;
  if (faceToEdge.find(fId) != faceToEdge.end())
    edgesOnF = faceToEdge.at(fId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id\n";
    std::cout << "Error: Model Face Id = " << fId << " does not exist\n";
    exit(1);
  }
 
  return edgesOnF;
}

