#include "xgc_reader_classification.hpp"

/****************************/
// Class MeshClassification
/****************************/
MeshClassification::MeshClassification(const Omega_h::Mesh& mesh):omegahMesh(mesh)
{
  setMeshVerticesClassification();
  setMeshEdgesClassification();
  setMeshFacesClassification();
}

// Function to set the classification of the mesh vertices.
void MeshClassification::setMeshVerticesClassification()
{
  auto vertId = omegahMesh.globals(Omega_h::VERT);
  auto vertClassDim = omegahMesh.get_array<int8_t>(0, "class_dim");
  auto vertClassId = omegahMesh.get_array<int>(0, "class_id");
  for (int i = 0; i < vertId.size(); i++)
  {
    int vId = vertId[i];
    verticesClassDim[vId] = static_cast<TopoType>(vertClassDim[i]);
    verticesClassId[vId] = vertClassId[i]; 
  }
}

// Function to set the classification of the mesh edges.
void MeshClassification::setMeshEdgesClassification()
{
  auto edgeId = omegahMesh.globals(Omega_h::EDGE);
  auto edgeClassDim = omegahMesh.get_array<int8_t>(1, "class_dim");
  auto edgeClassId = omegahMesh.get_array<int>(1, "class_id");

  for (int i = 0; i < edgeId.size(); i++)
  {
    int eId = edgeId[i];
    edgesClassDim[eId] = static_cast<TopoType>(edgeClassDim[i]);
    edgesClassId[eId] = edgeClassId[i]; 
  }
}

// Function to set the classification of the mesh faces.
void MeshClassification::setMeshFacesClassification()
{
  auto faceId = omegahMesh.globals(Omega_h::FACE);
  auto faceClassDim = omegahMesh.get_array<int8_t>(2, "class_dim");
  auto faceClassId = omegahMesh.get_array<int>(2, "class_id");

  for (int i = 0; i < faceId.size(); i++)
  {
    int fId = faceId[i];
    facesClassDim[fId] = static_cast<TopoType>(faceClassDim[i]);
    facesClassId[fId] = faceClassId[i]; 
  }
}

// Given the mesh vertex id, return the dimension/topology of the model entity
// on which mesh vertex is classified.
TopoType MeshClassification::getMeshVertexClassDim(const MeshIdType& vId) const
{
  TopoType t;
  if (verticesClassDim.find(vId) != verticesClassDim.end())
    t = verticesClassDim.at(vId);
  else
  {
    std::cout << "Error: Incorrect Mesh Vertex Id\n";
    std::cout << "Error: Mesh Vertex Id = " << vId << " does not exist\n";
    exit(1);
  }

  return t;
}

// Given the mesh edge id, return the dimension/topology of the model entity
// on which mesh edge is classified.
TopoType MeshClassification::getMeshEdgeClassDim(const MeshIdType& eId) const
{
  TopoType t;
  if (edgesClassDim.find(eId) != edgesClassDim.end())
    t = edgesClassDim.at(eId);
  else
  {
    std::cout << "Error: Incorrect Mesh Edge Id\n";
    std::cout << "Error: Mesh Edge Id = " << eId << " does not exist\n";
    exit(1);
  }
  
  return t;
}

// Given the mesh face id, return the dimension/topology of the model entit
// on which mesh face is classified.
TopoType MeshClassification::getMeshFaceClassDim(const MeshIdType& fId) const
{
  TopoType t;
  if (facesClassDim.find(fId) != facesClassDim.end())
    t = facesClassDim.at(fId);
  else
  {
    std::cout << "Error: Incorrect Mesh Face Id\n";
    std::cout << "Error: Mesh Face Id = " << fId << " does not exist\n";
    exit(1);
  }
  
  return t;
}

// Given the mesh vertex id, return the geometric id of the model entity
// on which mesh vertex is classified.
GeomIdType MeshClassification::getMeshVertexClassId(const MeshIdType& vId) const
{
  GeomIdType id;
  if (verticesClassId.find(vId) != verticesClassId.end())
    id = verticesClassId.at(vId);
  else
  {
    std::cout << "Error: Incorrect Mesh Vertex Id\n";
    std::cout << "Error: Mesh Vertex Id = " << vId << " does not exist\n";
    exit(1);
  }

  return id;
  
}

// Given the mesh edge id, return the geometric id of the model entity
// on which mesh edge is classified.
GeomIdType MeshClassification::getMeshEdgeClassId(const MeshIdType& eId) const
{
  GeomIdType id;
  if (edgesClassId.find(eId) != edgesClassId.end())
    id = edgesClassId.at(eId);
  else
  {
    std::cout << "Error: Incorrect Mesh Edge Id\n";
    std::cout << "Error: Mesh Edge Id = " << eId << " does not exist\n";
    exit(1);
  }
  
  return id;
}

// Given the mesh face id, return the geometric id  of the model entity
// on which mesh face is classified.
GeomIdType MeshClassification::getMeshFaceClassId(const MeshIdType& fId) const
{
  GeomIdType id;
  if (facesClassId.find(fId) != facesClassId.end())
    id = facesClassId.at(fId);
  else
  {
    std::cout << "Error: Incorrect Mesh Face Id\n";
    std::cout << "Error: Mesh Face Id = " << fId << " does not exist\n";
    exit(1);
  }
  
  return id;
}

