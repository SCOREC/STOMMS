#include "plane.h"

/***********************************************/
// Class: Plane
/***********************************************/

// Function to set field following points on all the flux curves of the plane.
void Plane::setFieldPointsOnFlux(const std::vector <FluxParametricPoints>& fieldPoints)
{
  fieldPointsOnFluxCurves = fieldPoints;
}

// Function to set model edges and vertices on the plane. We already have model faces
void Plane::setModelEntitiesFromModelFaces(const std::vector <Face>& modelFaces)
{
  // Use sets to avoid duplicates.
  std::set <Edge, CompEntity> edges;
  std::set <Vertex, CompEntity> vertices;

  // Step 1: Iterate over the model faces.
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];

    // Step 2: Iterate over the edges on the model face and save them 
    // in the set.
    std::vector <Edge> edgesOnFace = f.getEdgesOnFace();
    for (int j = 0; j < edgesOnFace.size(); j++)
      edges.insert(edgesOnFace[j]);
    
    // Step 3: Iterate over the vertices on the model face and save 
    // them in the set.
    std::vector <Vertex> verticesOnFace = f.getVerticesOnFace();
    for (int j = 0; j < verticesOnFace.size(); j++)
      vertices.insert(verticesOnFace[j]);
  }

  // Step 4: Assign the sets to the vectors in the class.
  modelEdges.assign(edges.begin(), edges.end());
  modelVertices.assign(vertices.begin(), vertices.end());  
}

// Function to set x-point using vertex of type pGVertex (Simmetrix).
void Plane::setXPoint(const pGVertex& gv)
{
  Vertex v;
  v.setSimVertex(gv);
  xPoints.push_back(v);
}

// Function to set desired mesh size on unstructured mesh faces.
void Plane::setUnstructuredMeshSizeOnPlane(double meshSize)
{
  meshSizeUnstructured = meshSize;
}

// Function to set model sizes on the model faces of the plane.
void Plane::setMeshSizeOnModelFaces(const std::unordered_map <int, double>& meshSizeOnF)
{
  meshSizeOnFaces = meshSizeOnF;
}

// Function to set x-point using vertex of type Vertex( STOMMS).
void Plane::setXPoint(const Vertex& v)
{
  xPoints.push_back(v);
}

// Function to get a vector of class that holds field following points info.
const std::vector <FluxParametricPoints>& Plane::getFieldPointsOnFluxCurves() const
{
  return fieldPointsOnFluxCurves;
}

// Function to get a vector of model edges on the plane.
const std::vector <Edge>& Plane::getModelEdgesOnPlane() const
{
  return modelEdges;
}

// Function to get a vector of model vertices on the plane.
const std::vector <Vertex>& Plane::getModelVerticesOnPlane() const
{
  return modelVertices;
}

// Function to get a vector of model vertices classified as xpoints.
const std::vector <Vertex>& Plane::getXPointsOnPlane() const
{
  return xPoints;
}

// Function to return desired mesh size of unstructured mesh faces.
const double& Plane::getUnstructuredMeshSizeOnPlane() const
{
  return meshSizeUnstructured;
}

// Function to get a map between model face ids and respective mesh sizes on them.
const std::unordered_map <int , double>& Plane::getMeshSizesOnModelFaces()
{
  return meshSizeOnFaces;
}
