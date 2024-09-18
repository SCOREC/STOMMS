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
    // Step 2.2: Iterate over the flux curves from the respective plane
    // and set the meshes.
    for (int i = 0; i < p.fluxCurves.size(); i++)
    {
      Flux f = p.fluxCurves[i];

      // Specify mesh entities (vertices and edges). 
      specifyMeshEnt(mesh, f, numSpecifiedVert);
    }

    // Step 2.3: Iterate over the model faces from the respective plane.
    for (int j = 0; j < p.modelFaces.size(); j++)
    {
      pGFace gf = p.modelFaces[j];

      // Step 2.3: Ensure there are no mesh vertices on the model face.
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

// Given the flux curve of type Flux, get the mesh size on this flux curve.
// Delete it in future. We are setting meshes byt specifying them on model edges.
double getMeshSizeOnFlux(Flux f)
{
   double fluxLength = 0.0;

   // Step 1: Iterate over the edges in this flux curve
   // and get total length of all edges.
   for (int i = 0; i < f.numEdgesOnFlux; i++)
   {
     // Step 1.1: Get the edge.
     pGEdge ge = f.edgesOnFlux[i];

     // Step 1.2: Add its length to total length.
     fluxLength += GE_length(ge);
   }
   
   // Step 2: Final mesh size on this flux curve is calcuated by dividing the total 
   // length of flux curve by dersired number of mesh vertices on this flux curve.
   double meshSizeOnFlux = fluxLength/f.meshVerticesOnFlux;
 
   return meshSizeOnFlux;
}

// To specify mesh vertices and edges on flux curves (model edges)
// Assumes periodic edges. Write a new function if edges are open 
// or have some other behaviour.
void specifyMeshEnt(pMesh mesh, Flux f, int& numSpecifiedVert)
{
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
  // numSpecifiedVert.
  pGVertex gv = GE_vertex(ge,0);
  currentPar[0] = parR[0];
  indx[0] = numSpecifiedVert++;
  int indxStart = indx[0];	// Since edge is periodic, we need this index for specifying last edge.
  MS_specifyVertex(mesh, 0, currentPar, gv, indx[0]);

  // Step 5: Start specifying vertices on the model edge, and also
  // specify mesh edges between them.
  int numIter = 1;  // Already placed one vertex
  while (numIter < numVert)
  {
    // Step 5.1: Update currentPar and indx for new verter.
    currentPar[0] += parInterval;
    indx[1] = numSpecifiedVert++;

    // Step 5.2: Specify the new mesh vertex.
    MS_specifyVertex(mesh,0,currentPar,ge,indx[1]);

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
}


