#include "stommsMesh.h"

// Given the simmetrix model and planes data, this function generates
// and return a simmetrix mesh.
StommsMesh::StommsMesh(const MeshMetaData& m):meshMetaData(m)
{
  // Step 1: get the Simmetrix model (pGModel) from MeshMetaData.
  StommsModel stommsModel = meshMetaData.getStommsModel();
  Model mdl = stommsModel.getModel();
  pGModel model = mdl.getSimModel();

  // Step 2: Initialize the simmetrix meshing objects required for meshing
  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);
  pMesh mesh = M_new(0, model); 
  pACase meshCase = MS_newMeshCase(model);

  // Step 3: Iterate over the planes container and set the mesh size
  // on mesh entites (model edges and faces).
  planes = meshMetaData.getMeshMetaDataPlanes();
  for (int i = 0; i < planes.size(); i++)
  {
    // Step 3.1: Fetch the desired plane.
    PlaneMeshMetaData p = planes[i];

    // Step 3.2: Specify mesh vertex at O-point of current plane and get the 
    // index of mesh vertex specified at the O-point.
    Vertex axis = p.getOPointOnPlane();
    pGVertex oPoint = axis.getSimVertex();
    int axisIndex = specifyMeshVertexOnModelVertex(mesh, oPoint);

    // Step 3.3: Set mesh properties on the flux curves.
    setMeshOnPlaneFluxCurves(mesh, p);

    // Step 3.5: Set mesh properties on the model faces.
    setMeshOnPlaneFaces(mesh, meshCase, p);
  }

  // Iterate over the model regions
  GRIter regions = GM_regionIter(model);
  if (GRIter_size(regions) > 0 ) 
    modelDim = 3;
  while (pGRegion rgn = GRIter_next(regions))
  {
    int meshType = 1; // 1 = tet, 2 = hex, 3 = mix
    MS_setVolumeMeshType(meshCase, rgn, meshType);
  } 
  GRIter_delete(regions);  
  std::cout << " ============ Meshing Starts ============\n";
 
  // Step 4: Execute the Simmetrix mesher 
  // Surface Mesher
  pSurfaceMesher surfMesh = SurfaceMesher_new(meshCase,mesh);
  SurfaceMesher_execute(surfMesh,prog);
  SurfaceMesher_delete(surfMesh);

  // Volume Mesher
  if (modelDim == 3)
  {
    pVolumeMesher volMesh = VolumeMesher_new(meshCase, mesh);
    VolumeMesher_execute(volMesh,prog);
    VolumeMesher_delete(volMesh);
  }
  MS_deleteMeshCase(meshCase);  

  // Step 5: Write the mesh to disk for visualization.
  M_write(mesh, "simMesh.sms", 0, prog);
  printMeshData(mesh);
  Progress_delete(prog);

  // Step 6: Save the mesh in StommsMesh class.
  simMesh = mesh;

  // Step 7: Setup the mesh data on planes to use it in output writing.
  setMeshDataOnPlanes();
}

void StommsMesh::setMeshOnPlaneFluxCurves(pMesh mesh, PlaneMeshMetaData& p)
{
  // Step 1: Iterate over the flux curves from the respective plane
  // and set the meshes. For first flux curve (j == 0), save the 
  // indices on flux curve f for specifying edges on model face.
  std::vector <Flux> fluxCurves = p.getFluxCurvesOnPlane();
  std::vector <FluxParametricPoints> parValuesOnFluxCurves = p.getMeshVerticesOnFlux();
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    // Step 1.1: Specify mesh entities (vertices and edges) on flux curves. 
    Flux f = fluxCurves[i];
    FluxParametricPoints parValuesOfVertices = parValuesOnFluxCurves[i];

    // TO-DO: FIRST FIND CURVE TYPE AND THEN CALL CLOSED OR ANY OTHER TYPE
    specifyMeshOnClosedCurve(mesh, f, parValuesOfVertices);
  } 
}

void StommsMesh::setMeshOnPlaneFaces(pMesh mesh, pACase meshCase, PlaneMeshMetaData& p)
{
  // Step 1: Iterate over the model faces from the respective plane.
  std::vector <Face> modelFaces = p.getModelFacesOnPlane();
  std::vector <int> faceMeshType = p.getFaceMeshType();
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];
    pGFace gf = f.getSimFace();
    
    int meshType = faceMeshType[i];

    if (meshType == 1)
      setOneElementDeepMeshOnFace(mesh, meshCase, gf);
    else
      MS_setMeshSize(meshCase, gf, 1, 0.01, 0);  // TO-DO: Get this mesh size directly from user input
  } 
}

void StommsMesh::setOneElementDeepMeshOnFace(pMesh mesh, pACase meshCase, pGFace gf)
{
  // Step 1: Ensure there are no mesh vertices on the model face.
  MS_ensureMeshSpansFace(meshCase, gf);  // ensures no vertex on the model face.
      
  // Step 2: Ensure not all mesh vertices of an element are on single model edge.
  pPList edgesOnFace = GF_edges(gf);  // get list of edges on the face.
  for (int j = 0; j < PList_size(edgesOnFace); j++)
  {
    pPList edgesList = PList_new();  // A list to store edges to pass to function "MS_preventAllFaceVerticesOnGEdges"
    pGEdge ge = static_cast<pGEdge>(PList_item(edgesOnFace, j)); 
    PList_append(edgesList, ge);
    MS_preventAllFaceVerticesOnGEdges(meshCase, gf, edgesList);
    PList_delete(edgesList);
  }
  PList_delete(edgesOnFace);
}

// To specify mesh vertex at O-point (origin/axis of the poloidal plane)
int StommsMesh::specifyMeshVertexOnModelVertex(pMesh mesh, pGVertex gv)
{
  // Step 1: Get the index to spcify the mesh vertex.
  int meshVertexIndex = numSpecifiedVert++;

  // Step 2: Get the location of the O-point.
  double xyz[3];  // location of O-point
  GV_point(gv, xyz);

  // Step 3: Specify the mesh vertex on the axis.
  MS_specifyVertex(mesh, xyz, 0, gv, meshVertexIndex);

  // Step 4: return the index.
  return meshVertexIndex;
}

// To specify mesh vertices and edges on flux curves (model edges)
// Assumes periodic edges. Write a new function if edges are open 
// or have some other behaviour.
void StommsMesh::specifyMeshOnClosedCurve(pMesh mesh, Flux f, const FluxParametricPoints& parValuesOnFlux)
{
  std::vector <Edge> edges = f.edgesOnFlux;
  if (edges.size() == 1 && edges[0].edgeIsPeriodic())
  {
    std::vector <double> parValuesOnEdge = parValuesOnFlux.getParametricValuesAtFluxEdge(edges[0]);
    specifyMeshOnPeriodicModelEdge(mesh, edges[0], parValuesOnEdge);
  }

  // Otherwise iterate over the edges
  // Start here tomorrow
}

void StommsMesh::specifyMeshOnPeriodicModelEdge(pMesh mesh, Edge edge, const std::vector<double>& parValues)
{
  int indxStart;

  // Step 1: Get the Simmetrix model edge.
  pGEdge ge = edge.getSimEdge();

  // Step 2: Start with first parametric value on the edge and  specify mesh vertex on it.
  // Keep updating numSpecifiedVert after every point.
  int indx[2];
  indx[0] = numSpecifiedVert++;
  double par[2] = {0.0, 0.0};
  par[0] = parValues[0];

  pPList vertices = GE_vertices(ge);
  if (PList_size(vertices) == 1)
  {
    pGVertex gv = static_cast<pGVertex>(PList_item(vertices,0));
    MS_specifyVertex(mesh, 0, par, gv, indx[0]);  
  }
  else
    MS_specifyVertex(mesh, 0, par, ge, indx[0]);

  PList_delete(vertices);
  indxStart = indx[0];  // save it for last specified edge.

  // Step 3: Loop over the remaining parametric values after the first one and also specify edges between 
  // every two specified mesh points.
  for (int i = 1; i < parValues.size(); i++)
  {
    indx[1] = numSpecifiedVert++;
    par[0] = parValues[i];

    // Step 3.1: Specify mesh vertex at each point.
    MS_specifyVertex(mesh,0,par,ge,indx[1]);

    // Step 3.2: Specify mesh edges between two consecutive specified points 
    // and update indx[0] for next iteration in loop. 
    MS_specifyEdge(mesh,indx,ge,-1);
    indx[0] = indx[1];
  }
  
  // Step 4: For the periodic model edges, the last mesh edge is between
  // last specified point (indx[0]) and starting point of the flux curve (indxOnFlux[0]).
  indx[1] = indxStart;
  MS_specifyEdge(mesh,indx,ge,-1);  
}

//void StommsMesh::specifyMeshOnModelEdge()

// To set up the mesh data on all the poloidal planes in the domain.
void StommsMesh::setMeshDataOnPlanes()
{
  // Iterate over all the planes and then set mesh data in each plane one by one.
  for (int i = 0; i < planes.size(); i++)
  {
    // Step 1: Get the mesh meta data on each plane.
    PlaneMeshMetaData plane = planes[i];

    // Step 2: Create an object of planer mesh data (PlaneMeshData).
    PlaneMeshData p;

    // Step 3: Feed input info to PlaneMeshData p.
    p.getMeshInfoOnPlane(plane, simMesh);

    // Step 4: Set up the plane and push it back to planes container.
    p.setMeshDataOnPlane();
    planesMeshData.push_back(p);    
  }
}


// Function to get underlying Simmetrix Mesh (pMesh) in StommsMesh.
const pMesh& StommsMesh::getSimMesh()
{
  return simMesh;
}

// Function to return mesh meta data associated with stomms mesh.
const MeshMetaData& StommsMesh::getMeshMetaData()
{
  return meshMetaData;
}

const std::vector <PlaneMeshData>& StommsMesh::getMeshDataOnPlanes()
{
  return planesMeshData;
}

int StommsMesh::getMeshDim()
{
  return modelDim;
}

/***********************************************/
// Class: PlaneMeshData
// To hold all the planer mesh data.
/***********************************************/

// Function to get mesh info needed to setup a mesh on plane.
void PlaneMeshData::getMeshInfoOnPlane(const PlaneMeshMetaData& plane, const pMesh& mesh)
{
  // Set up planer mesh meta data on the mesh plane. Its required to setup
  // mesh data on the plane.
  meshMetaDataOnP = plane;

  // Get the Simmetrix mesh needed to setup planer mesh data.
  simMeshGlobal = mesh;
}

// Function to set mesh data on a plane.
void PlaneMeshData::setMeshDataOnPlane()
{
  // Add model domain for the 2D plane
  set2DPlanerDomain();  

  // Add planer 2D mesh from the domain
  set2DPlanerMesh();
}

// Function to define a pGDomain for a plane.
void PlaneMeshData::set2DPlanerDomain()
{
  // Step 1: Set the model faces on a plane to a pPList.
  pPList gfOnDomain =  PList_new();
  std::vector<Face> modelFaces = meshMetaDataOnP.getModelFacesOnPlane();
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];
    pGFace gf = f.getSimFace();
    PList_append(gfOnDomain, gf);
  } 

  // Step 2: Define a model domain using the model faces in the pPList.
  // Set closure = 1 (3rd argument) to make sure domain contains the 
  // model edges and vertices on the poloidal plane.
  modelDomain = GDomain_new();
  GDomain_addModelEntities(modelDomain, gfOnDomain,1);

  PList_delete(gfOnDomain);
}

// Function to set pMesh on a plane defined by the pGDomain.
void PlaneMeshData::set2DPlanerMesh()
{
  // Get the simMesh (data member) on plane and 
  // save it in PlaneMeshData.
  simMesh =  M_copyDomain(simMeshGlobal, modelDomain);
 
  GDomain_delete(modelDomain);
}

// Function to return domain that defines a poloidal plane.
const pGDomain PlaneMeshData::getDomain()
{
  return modelDomain;
}

// Function to return Simmetrix mesh defined on a plane.
const pMesh PlaneMeshData::getMesh()
{
  return simMesh;
}

// Function to get global mesh back (contains all the poloidal planes).
const pMesh PlaneMeshData::getGlobalMesh()
{
  return simMeshGlobal;
}
// Function to return a vector of mesh vertices classified on geometric edge (ge).
std::vector <pVertex> getMeshVerticesOnModelEdge(pMesh m, pGEdge ge)
{
  std::vector <pVertex> vertices;

  // Simmetrix mesh vertex iterator on model edge ge.
  VIter vertexIter = M_classifiedVertexIter(m, ge, 0);
  while (pVertex v  = VIter_next(vertexIter))
    vertices.push_back(v);

  VIter_delete(vertexIter);
  return vertices;
}

// Function to return a vector of mesh vertices classified on geometric face (gf).
std::vector <pVertex> getMeshVerticesOnModelFace(pMesh m, pGFace gf)
{
  std::vector <pVertex> vertices;

  // Simmetrix mesh vertex iterator on model face gf.
  VIter vertexIter = M_classifiedVertexIter(m, gf, 0);
  while (pVertex v  = VIter_next(vertexIter))
    vertices.push_back(v);

  VIter_delete(vertexIter);
  return vertices;
}

// Function to return a vector of mesh edges classified on geometric edge (ge).
std::vector <pEdge> getMeshEdgesOnModelEdge(pMesh m, pGEdge ge)
{
  std::vector <pEdge> edges;

  // Simmetrix mesh edge iterator on model edge ge.
  EIter edgeIter = M_classifiedEdgeIter(m ,ge, 0);
  while (pEdge e = EIter_next(edgeIter))
    edges.push_back(e);

  EIter_delete(edgeIter);
  return edges;
}

// Function to return a vector of mesh edges classified on geometric face (gf).
std::vector <pEdge> getMeshEdgesOnModelFace(pMesh m, pGFace gf)
{
  std::vector <pEdge> edges;

  // Simmetrix mesh edge iterator on model face gf.
  EIter edgeIter = M_classifiedEdgeIter(m ,gf, 0);
  while (pEdge e = EIter_next(edgeIter))
    edges.push_back(e);

  EIter_delete(edgeIter);
  return edges;
}

// Function to return a vector of mesh faces classified on geometric face (gf).
std::vector <pFace> getMeshFacesOnModelFace(pMesh m, pGFace gf)
{
  std::vector <pFace> faces;

  // Simmetrix mesh face iterator on model face gf.
  FIter faceIter = M_classifiedFaceIter(m ,gf, 0);
  while (pFace f = FIter_next(faceIter))
    faces.push_back(f);

  FIter_delete(faceIter);
  return faces;
}

// Function to print mesh data. Right now just prints out 
// the number of mesh entities.
void printMeshData(const pMesh& mesh)
{
  std::cout << "================ Mesh Stats ================\n";
  std::cout << "Number of Vertices on the Mesh = " << M_numVertices(mesh) << "\n";
  std::cout << "Number of Edges on the Mesh = " << M_numEdges(mesh) << "\n";
  std::cout << "Number of Faces on the Mesh = " << M_numFaces(mesh) << "\n";
  std::cout << "Number of Regions on the Mesh = " << M_numRegions(mesh) << "\n";
}
