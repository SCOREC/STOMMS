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
  std::set <Edge, CompEntity> edges;
  std::set <Vertex, CompEntity> vertices;

  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];
    std::vector <Edge> edgesOnFace = f.getEdgesOnFace();
    for (int j = 0; j < edgesOnFace.size(); j++)
      edges.insert(edgesOnFace[j]);
    
    std::vector <Vertex> verticesOnFace = f.getVerticesOnFace();
    for (int j = 0; j < verticesOnFace.size(); j++)
      vertices.insert(verticesOnFace[j]);
  }
  modelEdges.assign(edges.begin(), edges.end());
  modelVertices.assign(vertices.begin(), vertices.end());  
}

void Plane::setXPoint(const pGVertex& gv)
{
  Vertex v;
  v.setSimVertex(gv);
  xPoints.push_back(v);
}

void Plane::setXPoint(const Vertex& v)
{
  xPoints.push_back(v);
}

// Function to get a vector of class that holds field following points info.
const std::vector <FluxParametricPoints>& Plane::getFieldPointsOnFluxCurves() const
{
  return fieldPointsOnFluxCurves;
}

const std::vector <Edge>& Plane::getModelEdgesOnPlane() const
{
  return modelEdges;
}

const std::vector <Vertex>& Plane::getModelVerticesOnPlane() const
{
  return modelVertices;
}

const std::vector <Vertex>& Plane::getXPointsOnPlane() const
{
  return xPoints;
}

