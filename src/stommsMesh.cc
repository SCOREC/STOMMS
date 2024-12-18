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
  std::vector <PlaneMeshMetaData> planes = meshMetaData.getMeshMetaDataPlanes();
  for (int i = 0; i < planes.size(); i++)
  {
    // Step 3.1: Fetch the desired plane.
    PlaneMeshMetaData p = planes[i];

    // Step 3.2: Specify mesh vertex at O-point of current plane and get the 
    // index of mesh vertex specified at the O-point.
    Vertex axis = p.getOPointOnPlane();
    pGVertex oPoint = axis.getSimVertex();
    int axisIndex = specifyMeshVertexOnAxis(mesh, oPoint);

    // Step 3.3: Iterate over the flux curves from the respective plane
    // and set the meshes. For first flux curve (j == 0), save the 
    // indices on flux curve f for specifying edges on model face.
    std::vector <Flux> fluxCurves = p.getFluxCurvesOnPlane();
    std::vector <std::vector<double>> meshVerticesParOnEdge = p.getMeshVerticesOnFlux();
    for (int j = 0; j < fluxCurves.size(); j++)
    {
      // Step 3.3.1: Specify mesh entities (vertices and edges) on flux curves. 
      Flux f = fluxCurves[j];
      std::vector <double> parValuesOfVertices = meshVerticesParOnEdge[j];
      std::vector <int> indxOnFlux = specifyMeshEnt(mesh, f, parValuesOfVertices);
    } 

    // Step 3.5: Iterate over the model faces from the respective plane.
    std::vector <Face> modelFaces = p.getModelFacesOnPlane();
    for (int j = 0; j < modelFaces.size(); j++)
    {
      Face f = modelFaces[j];
      pGFace gf = f.getSimFace();

      // Step 3.5.1: Ensure there are no mesh vertices on the model face.
      MS_ensureMeshSpansFace(meshCase, gf);  // ensures no vertex on the model face.
      
      // Step 3.5.2: Ensure not all mesh vertices of an element are on single model edge.
      pPList edgesOnFace = GF_edges(gf);  // get list of edges on the face.
      for (int k = 0; k < PList_size(edgesOnFace); k++)
      {
        pPList edgesList = PList_new();  // A list to store edges to pass to function "MS_setGEdgesToDisallowAllFaceVertices"
        pGEdge ge = static_cast<pGEdge>(PList_item(edgesOnFace, k)); 
        PList_append(edgesList, ge);
        MS_setGEdgesToDisallowAllFaceVertices(meshCase, gf, edgesList);
	PList_delete(edgesList);
      }
      PList_delete(edgesOnFace);
    } 
  }  
  std::cout << " ============ Meshing Starts ============\n";
 
  // Step 3: Execute the Simmetrix mesher 
  pSurfaceMesher surfMesh = SurfaceMesher_new(meshCase,mesh);
  SurfaceMesher_execute(surfMesh,prog);
  SurfaceMesher_delete(surfMesh);
  MS_deleteMeshCase(meshCase);  

  // Step 4: Write the mesh to disk for visualization
  M_write(mesh, "simMesh.sms", 0, prog);

  Progress_delete(prog);

  simMesh = mesh;
}

// To specify mesh vertex at O-point (origin/axis of the poloidal plane)
int StommsMesh::specifyMeshVertexOnAxis (pMesh mesh, pGVertex axis)
{
  // Step 1: Get the index to spcify the mesh vertex.
  int meshVertexIndex = numSpecifiedVert++;

  // Step 2: Get the location of the O-point.
  double xyz[3];  // location of O-point
  GV_point(axis, xyz);

  // Step 3: Specify the mesh vertex on the axis.
  MS_specifyVertex(mesh, xyz, 0, axis, meshVertexIndex);

  // Step 4: return the index.
  return meshVertexIndex;
}

// To specify mesh vertices and edges on flux curves (model edges)
// Assumes periodic edges. Write a new function if edges are open 
// or have some other behaviour.
std::vector <int> StommsMesh::specifyMeshEnt(pMesh mesh, Flux f, const std::vector<double>& parValuesOnFlux)
{
   std::vector <int> indxOnFlux;  // Indices of vertices spicified on the given flux curve (to return).

  // Step 1: Get the model edge and its parametric bounds.
  Edge e = f.edgesOnFlux[0];
  pGEdge ge = e.getSimEdge();

  // Step 2: Start with first parametric value on the edge and  specify mesh vertex on it.
  // Keep updating numSpecifiedVert after every point.
  int indx[2];
  indx[0] = numSpecifiedVert++;
  double par[2] = {0.0, 0.0};
  par[0] = parValuesOnFlux[0];
  MS_specifyVertex(mesh,0,par,ge,indx[0]);
  indxOnFlux.push_back(indx[0]);  // save indices of specified mesh vertices for return vector.

  // Step 3: Loop over the remaining parametric values after the first one and also specify edges between 
  // every two specified mesh points.
  for (int i = 1; i < parValuesOnFlux.size(); i++)
  {
    indx[1] = numSpecifiedVert++;
    par[0] = parValuesOnFlux[i];

    // Step 3.1: Specify mesh vertex at each point.
    MS_specifyVertex(mesh,0,par,ge,indx[1]);
    indxOnFlux.push_back(indx[1]);

    // Step 3.2: Specify mesh edges between two consecutive specified points 
    // and update indx[0] for next iteration in loop. 
    MS_specifyEdge(mesh,indx,ge,-1);
    indx[0] = indx[1];
  }
  
  // Step 4: For the periodic model edges, the last mesh edge is between
  // last specified point (indx[0]) and starting point of the flux curve (indxOnFlux[0]).
  indx[1] = indxOnFlux[0];
  MS_specifyEdge(mesh,indx,ge,-1);  

  // Step 7: returns the vector that contains the indices of specified mesh vertices on flux curve f.
  return indxOnFlux;
}

