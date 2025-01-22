#ifndef STOMMSMESH_H
#define STOMMSMESH_H

#include "modelData.h"
#include "meshMetaData.h"
#include <adios2.h>
#include <assert.h>

/*
 * Simmetrix function not yet available in any Simmetrix header. (2024-11-01).
 * Once its available, remove it from here.
 * See Simmetrix documentation for the usage of this function.
 */
 void MS_setGEdgesToDisallowAllFaceVertices(pACase cs, pGFace gf, pPList ges);

// class StommsMesh handles the meshing.
class StommsMesh{
  public:
    /*
     * Constructor takes the mesh meta data and mesh the StommsModel.
     * const MeshMetaData& m (in): Mesh meta data as input.
     */ 
    StommsMesh(const MeshMetaData& m);

    /*
     * Function to get underlying Simmetrix Mesh (pMesh).
     */ 
    const pMesh& getSimMesh();
  private:
    MeshMetaData meshMetaData;  // an object of class MeshMetaData
    std::vector <PlaneMeshMetaData> planes;  // a vector of planes holding mesh meta data
    pMesh simMesh;  // Simmetrix mesh
    int numSpecifiedVert = 0; // initializing the numSpecifiedVert here. It will be used
			      // in specifying mesh vertices on model entities.

    /*
     * To specify mesh vertex at O-point (origin/axis of the poloidal plane).
     * pMesh Mesh (in)(out): Gets the pMesh mesh as input and update the specified entities on it.
     * pGVertex axis (in): The model vertex at the O-point.
     * returns the index of the mesh vertex specified at the O-point.
     */
     int specifyMeshVertexOnAxis(pMesh mesh, pGVertex axis);

    /* To specify mesh vertices and edges on flux curves (model edges)
     * Assumes periodic edges. Write a new function if edges are open 
     * or have some other behaviour.
     * pMesh Mesh (in)(out): Gets the pMesh mesh as input and update the specified entities on it.
     * Flux f (in): The flux curve on which mesh entities are being specified.
     * const std::vector<double>& parValuesOnFlux (in): a vector holding parametric values of desired points on the flux curve.
     * returns a vector (int) that contains the indices of specified mesh vertices on flux curve f.
     */
     std::vector <int> specifyMeshEnt(pMesh mesh, Flux f, const std::vector<double>& parValuesOnFlux);
};

#endif
