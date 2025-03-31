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

/* 
 * Once Simmetrix mesh is generated, this class will hold the planer mesh data.
 */
class PlaneMeshData{
  public:
    /*
     * Function to get mesh info needed to setup a mesh on plane.
     * const PlaneMeshMetaData& plane (in): Mesh meta data on a plane. Needed to 
     * 					    extract mesh data on planes.
     *  const pMesh& mesh (in): Simmetrix mesh.
     */ 
    void getMeshInfoOnPlane(const PlaneMeshMetaData& plane, const pMesh& mesh);

    /*
     * Function to set mesh data on plane. Keeping it separate from setMeshEntitiesOnPlane()
     * in case we want to add more mesh data on plane and not just entities.
     */ 
    void setMeshDataOnPlane();

    /*
     * Function to return mesh vertices on a poloidal plane.
     */ 
    const std::vector <pVertex>& getMeshVerticesOnPlane();

    /*
     * Function to return mesh edges on a poloidal plane. 
     */ 
    const std::vector <pEdge>& getMeshEdgesOnPlane();

    /*
     * Function to return mesh faces on a poloidal plane.
     */ 
    const std::vector <pFace>& getMeshFacesOnPlane();

    /*
     * Function to return mesh regions on a poloidal plane.
     * The vector size of returned vector should be zero.
     */ 
    const std::vector <pRegion>& getMeshRegionsOnPlane();
  private:
    std::vector <pVertex> meshVonP;  // a vector to hold mesh vertices on a plane
    std::vector <pEdge> meshEonP;  // a vector to hold mesh edges on a plane
    std::vector <pFace> meshFonP;  // a vector to hold mesh faces on a plane
    std::vector <pRegion> meshRonP = {};  // a vector to hold mesh regions on a plane (no regions on a plane)

    pMesh simMeshGlobal;  // Simmetrix full domain mesh
    PlaneMeshMetaData meshMetaDataOnP;  // Mesh meta data on a plane. Needed to extract mesh data on planes.
    pGDomain modelDomain;
    pMesh simMesh;

    /*
     * Function to set mesh entities on a plane.
     */ 
    void setMeshEntitiesOnPlane();
    void set2DPlanerDomain();
    const pGDomain getDomain();
    void set2DPlanerMesh();
    const pMesh getMesh(); 
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
     * Function to return vector of poloidal planes holding mesh data.
     */   
    const std::vector <PlaneMeshData>& getMeshDataOnPlanes();

  /*
   * Function to return mesh dimension.
   */ 
    int getMeshDim(); 
  private:
    MeshMetaData meshMetaData;  // an object of class MeshMetaData
    int modelDim = 2;
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

     /*
      * To set up the mesh data on all the poloidal planes in the domain.
      */ 
     void setMeshDataOnPlanes();
};

/*
 * Function to get a vector of mesh vertices classified on geometric edge (ge).
 * pMesh m (in): Simmetrix mesh.
 * pGEdge ge (in): Geometric edge on the model. 
 * returns a vector of mesh vertices classified on ge in mesh m.
 */
std::vector <pVertex> getMeshVerticesOnModelEdge(pMesh m, pGEdge ge);

/*
 * Function to get a vector of mesh vertices classified on geometric face (gf).
 * pMesh m (in): Simmetrix mesh.
 * pGFace gf (in): Geometric face on the model. 
 * returns a vector of mesh vertices classified on gf in mesh m.
 */
std::vector <pVertex> getMeshVerticesOnModelFace(pMesh m, pGFace gf);

/*
 * Function to get a vector of mesh edges classified on geometric edge (ge).
 * pMesh m (in): Simmetrix mesh.
 * pGEdge ge (in): Geometric edge on the model. 
 * returns a vector of mesh edges classified on ge in mesh m.
 */
std::vector <pEdge> getMeshEdgesOnModelEdge(pMesh m, pGEdge ge);

/*
 * Function to get a vector of mesh edges classified on geometric face (gf).
 * pMesh m (in): Simmetrix mesh.
 * pGFace gf (in): Geometric face on the model. 
 * returns a vector of mesh edges classified on gf in mesh m.
 */
std::vector <pEdge> getMeshEdgesOnModelFace(pMesh m, pGFace gf);

/*
 * Function to get a vector of mesh faces classified on geometric face (gf).
 * pMesh m (in): Simmetrix mesh.
 * pGFace gf (in): Geometric face on the model. 
 * returns a vector of mesh faces classified on gf in mesh m.
 */
std::vector <pFace> getMeshFacesOnModelFace(pMesh m, pGFace gf);

/*
 * Function to print mesh data. Right now just prints out the number of mesh entities.
 * pMesh m (in): Simmetrix mesh.
 */
void printMeshData(const pMesh& mesh);

#endif
