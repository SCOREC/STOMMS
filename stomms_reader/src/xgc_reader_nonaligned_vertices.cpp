#include "xgc_reader_nonaligned_vertices.hpp"

/****************************/
// Class NonAlignedVertex
// for a single vertex
/****************************/
 
NonAlignedVertex::NonAlignedVertex(int vertexNumber, CurveIdType flux1, CurveIdType flux2)
{
  vertex = vertexNumber,
  fluxCurves.push_back(flux1);
  fluxCurves.push_back(flux2);
}

// Function to return the id of the non-aligned mesh vertex.
const int& NonAlignedVertex::getVertex() const
{
  return vertex;
}

// Function to return both bounding flux curves on the face that has non-aligned 
// mesh vertex on it.
const std::vector <CurveIdType>& NonAlignedVertex::getBoundingFluxCurves() const
{
  assert(fluxCurves.size() == 2);
  return fluxCurves;
}

/****************************/
// Class NonAlignedVertices
// To set vertices in a plane
/****************************/

NonAlignedVertices::NonAlignedVertices(const Omega_h::Mesh& omegahMesh, const Model& inputModel, 
                                       const ModelTopology& modelTopology,
                                       const ReverseClassification& reverseClassification)
{
  // Step 1: Set the local variables
  model = inputModel;
  mesh = omegahMesh;
  modelTopo = modelTopology;
  revClass = reverseClassification;

  // Step 2: Iterate over the model faces, and check if they need to be considered to find 
  // non-aligneed vertices
  std::map <SurfaceIdType, ModelFace> modelFaces = model.getModelFaces(); 
  for (const auto &face : modelFaces)
  {
    SurfaceIdType faceId = face.first;
    ModelFace modelFace = face.second;

    std::vector <NonAlignedVertex> vertices;
    if (!considerModelFaceForNonAlignedVertices(modelFace))
    {
      nonAlignedVerticesOnFace[faceId] = vertices;
      continue;
    }
   
    vertices = setNonAlignedVerticesAtFace(faceId);
    nonAlignedVerticesOnFace[faceId] = vertices;
  }

}

// Function to evaluate if a model face should be considered for non-aligned mesh 
// vertices search or not.
bool NonAlignedVertices::considerModelFaceForNonAlignedVertices(const ModelFace& modelFace)
{
  bool faceForNonAlignedVertices = false;
  ModelFace f = modelFace;
  SurfaceType sType = f.getSurfaceType();
  SurfaceIdType sId = f.getGeomId();

  // Step 1: Ignore the model faces on low/high near vacuum side regions. 
  if (sType == SurfaceType::LowFieldSideNearVacuum || sType == SurfaceType::HighFieldSideNearVacuum)
    return false;
  
  // Step 2:Find out psi values on a model face (using psi values on edges).  If a face has bounding 
  // model edges with two psi values, consider it for further processing otherwise return false.
  std::set<double> psiSet;
  std::vector <EdgeIdType> edgesOnF = modelTopo.getAdjacentEdgesOnFace(sId);
  for (int i = 0; i < edgesOnF.size(); i++)
  {
    CurveIdType curveId = model.getModelCurveFromEdgeId(edgesOnF[i]);
    ModelCurve curve = model.getModelCurveFromId(curveId);
    double psi = curve.getCurvePsi();
    if (psi < 0.0)
      continue; // wallCurve, psi = -1.0
    psiSet.insert(psi);   
  }

  if (psiSet.size()!=2)
    faceForNonAlignedVertices = false;
  else
    faceForNonAlignedVertices = true;

  return faceForNonAlignedVertices;  
}

// Function to return a vector of model edges on a model face to be considered for non-aligned vertices search.
std::vector <EdgeIdType> NonAlignedVertices::considerModelEdgesForNonAlignedVertices(const SurfaceIdType& faceId)
{
  std::vector <EdgeIdType> edgesToConsider;
  std::vector <EdgeIdType> edgesOnF = modelTopo.getAdjacentEdgesOnFace(faceId);
 
  // Step 1: Iterate over the model edges on the model face, and only consider the ones
  // that are on wall curve.
  for (int i = 0; i < edgesOnF.size(); i++)
  {
    CurveIdType curveId = model.getModelCurveFromEdgeId(edgesOnF[i]);
    ModelCurve curve = model.getModelCurveFromId(curveId);
    CurveType curveType = curve.getCurveType();
    if (curveType != CurveType::Wall)
      continue;
   
    edgesToConsider.push_back(edgesOnF[i]);
  }

  return edgesToConsider;
}

// Function to return a vector of model vertices on a model face to be considered for non-aligned vertices search.
std::vector <VertexIdType> NonAlignedVertices::considerModelVerticesForNonAlignedVertices(const SurfaceIdType& faceId)
{
  std::vector <VertexIdType> verticesToConsider;
  std::vector <VertexIdType> verticesOnF = modelTopo.getAdjacentVerticesOnFace(faceId);

  // Step 1: Iterate over the model vertices on the model face.
  for (int i = 0; i < verticesOnF.size(); i++)
  {
    VertexIdType vertexId = verticesOnF[i];
    std::vector <EdgeIdType> edgesOnV = modelTopo.getAdjacentEdgesOnVertex(vertexId);

    // Step 2: If vertex has no adjacent model edges (as in OPoint), ignore it.
    if (edgesOnV.size() == 0)
      continue;

    // Step 3: Iterate over the edges adjacent to the model vertex, and if the vertex
    // had adjacent flux curves (closed, open , or separatrix), ignore such vertex. 
    bool vertexToConsider = true;
    for (int j = 0; j < edgesOnV.size(); j++)
    {
      CurveIdType curveId = model.getModelCurveFromEdgeId(edgesOnV[j]);
      ModelCurve curve = model.getModelCurveFromId(curveId);
      CurveType curveType = curve.getCurveType();
      if (curveType == CurveType::Closed || curveType == CurveType::Separatrix
          || curveType == CurveType::Open)
      {
        vertexToConsider = false;
        break;
      }
    }
   
    if (vertexToConsider)
      verticesToConsider.push_back(vertexId);
  }

  return verticesToConsider;
}

// Given the id of model face, set the non-aligned mesh vertices on it.
std::vector <NonAlignedVertex> NonAlignedVertices::setNonAlignedVerticesAtFace(const SurfaceIdType& faceId)
{
  std::vector <NonAlignedVertex> nonAlignedVertices;
  coords = mesh.coords();  // coordinates of mesh vertices from omegah mesh

  // Step 1: Filter out the model edges and vertices on this model face to be considered for
  // non-aligned vertices 
  std::vector <EdgeIdType> modelEdges = considerModelEdgesForNonAlignedVertices(faceId);
  std::vector <VertexIdType> modelVertices = considerModelVerticesForNonAlignedVertices(faceId);

  // Step 2: Get all the mesh vertices classified on model face, model edges and model vertices
  // Step 2.1: Find mesh vertices on model face and store them in meshVertices
  std::vector <MeshIdType> meshVertices = revClass.getFaceToVertexReverseClassification(faceId, false);

  // Step 2.2: Find mesh vertices on all model edges and append them to meshVertices
  for (int i = 0; i < modelEdges.size(); i++)
  {
    EdgeIdType edgeId = modelEdges[i];
    std::vector <MeshIdType> meshVerticesOnEdge = revClass.getEdgeToVertexReverseClassification(edgeId, false);
    meshVertices.insert(meshVertices.end(), meshVerticesOnEdge.begin(), meshVerticesOnEdge.end());
  }
  
  // Step 2.3: Find mesh vertices on all model vertices and append them to meshVertices
  for (int i = 0; i < modelVertices.size(); i++)
  {
    VertexIdType vertexId = modelVertices[i];
    std::vector <MeshIdType> meshVertexOnVertex = revClass.getVertexToVertexReverseClassification(vertexId);
    meshVertices.insert(meshVertices.end(), meshVertexOnVertex.begin(), meshVertexOnVertex.end());
  }

  // Step 2.4: If mesh vertices are zero for particular face, set an empty vector for consistency
  std::vector <NonAlignedVertex> nonAlignedVector = {};  //Empty vector just for consistency
  if (meshVertices.size() == 0)
    nonAlignedVerticesOnFace[faceId] = nonAlignedVector;
  
  // Step 3: Iterate over the mesh vertices, and find the two flux curves there are closest to on model face.
  for (int i = 0; i < meshVertices.size(); i++)
  {
    MeshIdType meshVertexId = meshVertices[i];
    std::array <double,2> vCoord = {coords[meshVertexId*2], coords[(meshVertexId*2)+1]};

    std::array <CurveIdType,2> fluxIds = getNearestBoundingFluxCurves(faceId, vCoord); 
    NonAlignedVertex nonAlignedVertex(meshVertexId, fluxIds[0], fluxIds[1]);
    nonAlignedVertices.push_back(nonAlignedVertex);
  }

  return nonAlignedVertices;
}

// Given the vertex coordinates and model face it is classified on, find the two flux curves with different psi values 
// which are closest to this vertex.
std::array <CurveIdType, 2> NonAlignedVertices::getNearestBoundingFluxCurves(const SurfaceIdType& faceId, const std::array<double, 2>& vertexCoords)
{
  // Step 1: Identify the edges with different psi values and put them in two separate containers
  std::vector <EdgeIdType> edgesOnF = modelTopo.getAdjacentEdgesOnFace(faceId);
  std::vector <EdgeIdType> edgesOnPsi1, edgesOnPsi2;
  double psi1;
  bool psi1Found = false;
  for (int i = 0; i < edgesOnF.size(); i++)
  {
    EdgeIdType edgeId = edgesOnF[i];
    CurveIdType curveId = model.getModelCurveFromEdgeId(edgeId);
    ModelCurve curve = model.getModelCurveFromId(curveId);
    CurveType curveType = curve.getCurveType();   
    if (curveType != CurveType::Closed && curveType != CurveType::Separatrix &&
        curveType != CurveType::Open) continue;
    
    double psi = curve.getCurvePsi();
    if (!psi1Found)
    {
      psi1 = psi;
      edgesOnPsi1.push_back(edgeId);
      psi1Found = true;
      continue;
    }
    
    if (fabs(psi1 - psi) < 1e-16)
      edgesOnPsi1.push_back(edgeId);
    else
      edgesOnPsi2.push_back(edgeId);
  }

  // Step 2: From the set of edges, find the one closest to the mesh vertex
  EdgeIdType edge1 = getNearestModelEdgeToPoint(edgesOnPsi1, vertexCoords);
  EdgeIdType edge2 = getNearestModelEdgeToPoint(edgesOnPsi2, vertexCoords);

  // Step 3: Find the curves on which these edges lies.
  CurveIdType curve1 = model.getModelCurveFromEdgeId(edge1);
  CurveIdType curve2 = model.getModelCurveFromEdgeId(edge2);
  
  std::array <CurveIdType, 2> boundingFluxCurves = {curve1, curve2};
  return boundingFluxCurves;
}

// Given the coordinates of a point, and a vector of model edges, find the model edges nearest to the point.
EdgeIdType NonAlignedVertices::getNearestModelEdgeToPoint(const std::vector<EdgeIdType> edges, const std::array<double, 2>& vertexCoords)
{
  double minimumDist = 1e16;
  EdgeIdType closestEdge;

  // Step 1: If there is only one edge, don't do further computation
  if (edges.size() == 1)
    return edges[0];

  for (int i = 0; i < edges.size(); i++)
  { 
    EdgeIdType edgeId = edges[i];
    double dist = getMinimumDistanceFromPointToEdge(edgeId, vertexCoords);  
    if (dist < minimumDist)
    {
      minimumDist = dist;
      closestEdge = edgeId;
    }
  }

  return closestEdge;
}

// Given the coordinates of a point, and an model edge, find the nearest possible distance from the edge to the point.
double NonAlignedVertices::getMinimumDistanceFromPointToEdge(EdgeIdType edge, const std::array<double, 2>& vertexCoords)
{
  std::vector <MeshIdType> verticesOnEdge = revClass.getEdgeToVertexReverseClassification(edge, true);
  double minimumDist = 1e16;
  for (int i = 0; i < verticesOnEdge.size(); i++)
  {
    MeshIdType meshVertexId = verticesOnEdge[i];
    std::array<double,2>  pt = {coords[meshVertexId*2], coords[(meshVertexId*2)+1]};
    double dist = dist2DPointToPoint(pt, vertexCoords);
    if (dist < minimumDist)
      minimumDist = dist;
  }

  return minimumDist;
}

std::vector <NonAlignedVertex> NonAlignedVertices::getNonAlignedVerticesOnFace(SurfaceIdType faceId) const
{
  std::vector <NonAlignedVertex> verticesOnF;
  if (nonAlignedVerticesOnFace.find(faceId) != nonAlignedVerticesOnFace.end())
    verticesOnF = nonAlignedVerticesOnFace.at(faceId);
  else
  {
    std::cout << "Error: Incorrect Model Face Id\n";
    std::cout << "Error: Model Face Id = " << faceId << " does not exist\n";
    exit(1);
  }
  
  return verticesOnF;
}

const std::map <SurfaceIdType, std::vector <NonAlignedVertex>>& NonAlignedVertices::getNonAlignedVerticesOnModel() const
{
  return nonAlignedVerticesOnFace;
} 
