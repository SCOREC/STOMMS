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

    // Step 2.2: Iterate over the model faces from the respective plane.
    for (int face = 0; face < p.modelFaces.size(); face++)
    {
      pGFace gf = p.modelFaces[face];

      // Step 2.3:  Set mesh size on the model face.
      MS_setMeshSize(meshCase, gf, 1, a->meshSize, 0);
      MS_ensureMeshSpansFace(meshCase, gf);  // ensures no vertex on the model face.
      int meshSizeSet = 0;

      // Step 2.4: Iterate over the model edges on the model face.
      pPList edges = GF_edges(gf);
      for (int i = 0; i< PList_size(edges); ++i)
      {
        pGEdge ge = static_cast<pGEdge>(PList_item(edges, i));
        GEN_nativeIntAttribute(ge,"meshSet", &meshSizeSet);

        // Step 2.5: If mesh size is already set, look for next edge.
        if (meshSizeSet == 1)
          continue;

        // Step 2.6: Set the mesh size on the edge and set the flag meshSet to 1 
        // which shows that mesh size is set on this model entity.
        MS_setMeshSize(meshCase, ge, 1, a->meshSize, 0);
        GEN_setNativeIntAttribute(ge, 1, "meshSet");
      }
      PList_delete(edges);
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
