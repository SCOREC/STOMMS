#ifndef STOMMSMESH_H
#define STOMMSMESH_H

#include "modelData.h"
#include "meshMetaData.h"
#include <assert.h>

/*
 * Simmetrix function not yet available in any Simmetrix header. (2024-11-01).
 * Once its available, remove it from here.
 * See Simmetrix documentation for the usage of this function.
 */
 void MS_setGEdgesToDisallowAllFaceVertices(pACase cs, pGFace gf, pPList ges);

// Once Simmetrix mesh is generated, this class will hold the planer mesh data.
class PlaneMeshData{
  public:
    void getMeshInfoOnPlane(const PlaneMeshMetaData& plane, const pMesh& mesh);
    void setMeshDataOnPlane();
    const std::vector <pVertex>& getMeshVerticesOnPlane();
    const std::vector <pEdge>& getMeshEdgesOnPlane();
    const std::vector <pFace>& getMeshFacesOnPlane();
    const std::vector <pRegion>& getMeshRegionsOnPlane();
  private:
    std::vector <pVertex> meshVonP;
    std::vector <pEdge> meshEonP;
    std::vector <pFace> meshFonP;
    std::vector <pRegion> meshRonP = {};

    pMesh simMesh;
    PlaneMeshMetaData meshMetaDataOnP;

    void setMeshEntitiesOnPlane();
};

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

    /*
     * Function to return mesh meta data associated with stomms mesh.
     */
    const MeshMetaData& getMeshMetaData();    

    /*
     * Function to get vector of  mesh data on each plane.
     */   
    const std::vector <PlaneMeshData>& getMeshDataOnPlane();
  private:
    MeshMetaData meshMetaData;  // an object of class MeshMetaData
    std::vector <PlaneMeshMetaData> planes;  // a vector of planes holding mesh meta data
    std::vector <PlaneMeshData> planesMeshData;  // a vector of planes holding simmetrix mesh data
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

     void setMeshDataOnPlanes();
};


std::vector <pVertex> getMeshVerticesOnModelEdge(pMesh m, pGEdge ge);
std::vector <pVertex> getMeshVerticesOnModelFace(pMesh m, pGFace gf);
std::vector <pEdge> getMeshEdgesOnModelEdge(pMesh m, pGEdge ge);
std::vector <pEdge> getMeshEdgesOnModelFace(pMesh m, pGFace gf);
std::vector <pFace> getMeshFacesOnModelFace(pMesh m, pGFace gf);
void printMeshData(const pMesh& mesh);

#endif
