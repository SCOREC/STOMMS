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

  // Step 2: Iterate over the planes container and set the mesh size
  // on mesh entites (model edges and faces).
  for (int i = 0; i < planes.size(); i++)
  {
    // Step 2.1: Fetch the desired plane.
    Plane p = planes[i];

    // Step 2.2: Iterate over the flux curves from the respective plane.
    for (int i = 0; i < p.fluxCurves.size(); i++)
    {
      Flux f = p.fluxCurves[i];

      // Step 2.2.1: Get the desired mesh size for each flux curve.
      double meshSize = getMeshSizeOnFlux(f);

      // Step 2.2.2: Set the mesh size of respective edges of flux curve.
      for (int k = 0; k < f.numEdgesOnFlux; k++)
      {
        pGEdge ge = f.edgesOnFlux[k];
        MS_setMeshSize(meshCase, ge, 1, meshSize, 0);
      }
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
 
   // For Debug, delete later when Simmodeler is available for visualization of results.
   //std::cout << "Psi = " << f.psiNormOnFlux << " , Number of Edges = " << f.numEdgesOnFlux << " , Desired Vertices = " << f.meshVerticesOnFlux << " , Total Length = " << fluxLength << " ,Mesh Size = " << meshSizeOnFlux << "\n";
       
   return meshSizeOnFlux;
}
