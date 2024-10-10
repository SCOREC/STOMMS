#include "meshing.h"

// Given the simmetrix model and planes data, this function generates
// and return a simmetrix mesh.
pMesh meshing(pGModel model, std::vector <Plane> planes, args* a)
{
  // Step 1: Initialize the simmetrix meshing objects required for meshing
  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);
  pMesh mesh = M_new(0, model); 
  pACase meshCase = MS_newMeshCase(model);

  // To keep the count of total specified vertices. We need global indices
  // of specified vertices when specifying mesh edges.
  int numSpecifiedVert = 0;	

  // Step 2: Iterate over the planes container and set the mesh size
  // on mesh entites (model edges and faces).
  for (int i = 0; i < planes.size(); i++)
  {
    // Step 2.1: Fetch the desired plane.
    Plane p = planes[i];

    // Step 2.2: Specify mesh vertex at O-point of current plane and get the 
    // index of mesh vertex specified at the O-point.
    int axisIndex = specifyMeshVertexOnAxis(mesh, p.oPoint, numSpecifiedVert);

    // Step 2.3: Iterate over the flux curves from the respective plane
    // and set the meshes. For first flux curve (j == 0), save the 
    // indices on flux curve f for specifying edges on model face.
    std::vector <int> indxOnFirstFlux;
    for (int j = 0; j < p.fluxCurves.size(); j++)
    {
      // Step 2.3.1: Specify mesh entities (vertices and edges) on flux curves. 
      Flux f = p.fluxCurves[j];
      std::vector <int> indxOnFlux = specifyMeshEnt(mesh, f, numSpecifiedVert, a);

      // Step 2.3.2: Save the indices of specified vertices on the first flux curve.
      if (j == 0)
        indxOnFirstFlux = indxOnFlux;
    }

    // Step 2.4: Specify mesh edges on model face adjacent to O-point.
    specifyMeshEdgesOnFace(mesh, p.oPoint, axisIndex, indxOnFirstFlux);

    // Step 2.5: Iterate over the model faces from the respective plane.
    for (int j = 0; j < p.modelFaces.size(); j++)
    {
      pGFace gf = p.modelFaces[j];

      // Step 2.5.1: Ensure there are no mesh vertices on the model face.
      MS_ensureMeshSpansFace(meshCase, gf);  // ensures no vertex on the model face.
      int meshSizeSet = 0;
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

  return mesh;
}

// To specify mesh vertex at O-point (origin/axis of the poloidal plane)
int specifyMeshVertexOnAxis (pMesh mesh, pGVertex axis, int& numSpecifiedVert)
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
std::vector <int> specifyMeshEnt(pMesh mesh, Flux f, int& numSpecifiedVert, args* a)
{
   std::vector <int> indxOnFlux;  // Indices of vertices spicified on the given flux curve (to return).
   
  // Step 1: Get the model edge and its parametric bounds.
  pGEdge ge = f.edgesOnFlux[0];
  double parR[2];  //par[0] = start, par[1] = end.
  GE_parRange(ge, &parR[0], &parR[1]);

  // Step 2: Based on number of desired mesh vertices on flux curve, 
  // define the parametric interval between mesh vertices.
  int numVert = f.meshVerticesOnFlux;
  double parInterval = (parR[1] - parR[0])/numVert;

  // Step 3: Define variables that will need to be updated with every specified vertex 
  // and edges. currentPar tells the parametric position where mesh vertex needs to be
  // specified.  Only currentPar[0] is relevant. currentPar[1] isn't required in our case.
  // Indx keeps track of index of specified vertices. Need indx for specifying mesh edges. 
  double currentPar[2] = {0.0,0.0};
  int indx[2];

  // Step 4: Specify first mesh vertex on starting model vertex of the edge ge.
  // currentPar is starting point of edge. Indx is global value comming from
  // numSpecifiedVert. Also, save the indx to the vector indxOnFlux.
  pGVertex gv = GE_vertex(ge,0);
  currentPar[0] = parR[0];
  indx[0] = numSpecifiedVert++;
  int indxStart = indx[0];	// Since edge is periodic, we need this index for specifying last edge.
  MS_specifyVertex(mesh, 0, currentPar, gv, indx[0]);
  indxOnFlux.push_back(indx[0]);

  // Step 5: Start specifying vertices on the model edge, and also
  // specify mesh edges between them.
  int numIter = 1;  // Already placed one vertex
  while (numIter < numVert)
  {
    // Step 5.1: Update currentPar and indx for new verter.If reading the field 
    // following points file, use par directly from the function.
    if (a->fieldDataFileOn == 1)
    {
      double parValue = parValueOnEdge(f, numIter);
      currentPar[0] = parValue;
    }
    else
      currentPar[0] += parInterval;
    indx[1] = numSpecifiedVert++;

    // Step 5.2: Specify the new mesh vertex and save it to indxOnFlux vector.
    MS_specifyVertex(mesh,0,currentPar,ge,indx[1]);
    indxOnFlux.push_back(indx[1]);

    // Step 5.3: Specify mesh edge between vertices with indices
    // indx[0] and indx[1].
    MS_specifyEdge(mesh,indx,ge,-1);

    // Step 5.4: Update the index, as for next mesh edge, the 
    // current ending vertex will be the starting vertex.
    indx[0] = indx[1];
    numIter++;
  }

  // Step 6: Specify last mesh edge on the model edge ge. Since model edge is periodic, last specified 
  // edge is between last specified vertex and first specified vertex on the model edge ge. 
  indx[1] = indxStart;  // First specified vertex on the model edge.
  MS_specifyEdge(mesh,indx,ge,-1);

  // Step 7: returns the vector that contains the indices of specified mesh vertices on flux curve f.
  return indxOnFlux;
}

// To specify mesh edges on the model face that is adjacent to the O-point.
void specifyMeshEdgesOnFace(pMesh mesh, pGVertex axis, int indxAtAxis, std::vector <int> indicesOnInnermostFlux)
{
  // Step 1: Get the model faces that are adjacent to model axis (O-point).Number of faces should 
  // be 1. Once face is saved, delete the list.
  pPList facesOnAxis = GV_faces(axis);
  assert(PList_size(facesOnAxis) == 1);
  pGFace gf = static_cast<pGFace>(PList_item(facesOnAxis,0));
  PList_delete(facesOnAxis);

  // Step 2: Setup the indices for the edge. The first index should always be mesh vertex
  // on the axis. For the second index, we will traverse the innermost flux curve.
  int indx[2] = {indxAtAxis, 0};

  // Step 3: Iterate over the indices vector for indx[1], and specify the edge on the 
  // model face gf.
  for (int i = 0; i < indicesOnInnermostFlux.size(); i++)
  {
    indx[1] = indicesOnInnermostFlux[i];
    MS_specifyEdge(mesh, indx, gf, -1); 
  }
}


// For a given flux curve, and the index of the field following point,
// return the parametric value of point on the edge.
double parValueOnEdge(Flux f, int indx)
{
  // Step 1: Get the model edge on the flux curve.
  pGEdge ge = f.edgesOnFlux[0];

  // Step 2: Find the coordinate of th field following point.
  double pt[3];
  pt[0] = f.pts[indx].x;
  pt[1] = f.pts[indx].y;
  pt[2] = f.pts[indx].z;

  // Step 3: Get the closest point on the edge from the field following point
  // This is to ensure that field following point is on the edge and if not
  // get that closest point on the edge.
  double closestPt[3],par;
  GE_closestPoint(ge, pt, closestPt, &par);

  // Step 4: Retunr the parametric value of that point.
  return par;
}
