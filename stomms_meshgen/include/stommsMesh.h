#ifndef STOMMSMESH_H
#define STOMMSMESH_H

#include "meshMetaData.h"
#include <assert.h>
#include <unordered_map>

/* 
 * Once Simmetrix mesh is generated, this class will hold the planer mesh data.
 */
class PlaneMeshData{
  public:
    /**
     * Function to get mesh info needed to setup a mesh on plane.
     * @param plane: Mesh meta data on a plane. Needed to extract 
     * mesh data on planes.
     * @param mesh: Simmetrix mesh.
     */ 
    void getMeshInfoOnPlane(const PlaneMeshMetaData& plane, const pMesh& mesh);

    /*
     * Function to set mesh data on plane. Keeping it separate from setMeshEntitiesOnPlane()
     * in case we want to add more mesh data on plane and not just entities.
     */ 
    void setMeshDataOnPlane();

    /*
     *  Function to get global mesh back (contains all the poloidal planes).
     */ 
    const pMesh getGlobalMesh();

    /*
     * Function to return mesh defined on a plane.
     */ 
    const pMesh getMesh();

    /*
     * Function to return domain that defines a poloidal plane.
     */ 
    const pGDomain getDomain();
  private:
    pMesh simMeshGlobal;  // Simmetrix full domain mesh
    PlaneMeshMetaData meshMetaDataOnP;  // Mesh meta data on a plane. Needed to extract mesh data on planes.
    pGDomain modelDomain;  //Domain defined by the model faces, edges, vertices on a single plane
    pMesh simMesh;  // planer mesh defined on the domain

    /*
     * Function to define a pGDomain for a plane. This function sets the model faces on a plane
     * including closure (edge, and vertices) to the domain.
     *
     */ 
    void set2DPlanerDomain();

    /*
     * Function to set pMesh on a plane defined by the pGDomain.
     */ 
    void set2DPlanerMesh();
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
    std::unordered_map<pGVertex, int> specifiedVertices;  // mesh vertices specified on model vertices.
    int numSpecifiedVert = 0; // initializing the numSpecifiedVert here. It will be used
			      // in specifying mesh vertices on model entities.

    /**
     * To specify mesh vertex at model vertex (origin/axis/other vertices of the poloidal plane).
     * @param mesh: Gets the pMesh mesh as input and update the specified entities on it.
     * @param gv: model vertex on which mesh vertex is desired
     * @return the index of the mesh vertex specified at the O-point.
     */
     int specifyMeshVertexOnModelVertex(pMesh mesh, pGVertex gv);

    /** 
     * To specify mesh vertices and edges on flux curves (model edges).
     * Top level function which calls implementation level functions. 
     * @param mesh: Gets the pMesh mesh as input and update the specified entities on it.
     * @param f: The flux curve on which mesh entities are being specified.
     * @param parValuesOnFlux: a vector holding class that contains parametric values of desired points 
     *                         on the flux curve.
     * returns a vector (int) that contains the indices of specified mesh vertices on flux curve f.
     */
     void specifyMeshOnFluxCurve(pMesh mesh, const Flux& f, const FluxParametricPoints& parValuesOnFlux);
  
     /**
      * To specify mesh vertices and edges on a periodic model edge.
      * @param mesh: Simmetrix mesh.
      * @param edge: periodic model edge on which mesh vertices/edges are going to be specified.
      * @param parValues: a vector of par values on which vertices need to be specified.
      */ 
     void specifyMeshOnPeriodicModelEdge(pMesh mesh, Edge edge, const std::vector<double>& parValues);

     /**
      * To specify mesh vertices and edges on a regular (non-periodic) model edge.
      * @param edge: periodic model edge on which mesh vertices/edges are going to be specified.
      * @param parValues: a vector of par values on which vertices need to be specified.
      */ 
     void specifyMeshOnModelEdge(pMesh mesh, Edge edge, const std::vector<double>& parValues);  // non-periodic

     /**
      * Function to return already specified vertex tag. If not specified yet, returns -1.
      * @param gv: model vertex on which specified vertex index is desired.
      * @return index of specified vertex if already specified, else -1.
      */ 
     int getSpecifiedVertexTag(const pGVertex& gv);

     /**
      * Given a plane p, and mesh, set mesh properties on the flux curves on the plane.
      * @param mesh: Simmetrix mesh.
      * @param p: a class to hold the mesh meta data of a plane.
      */
     void setMeshOnPlaneFluxCurves(pMesh mesh, PlaneMeshMetaData& p); 

     /**
      * Given a plane, mesh, and mesh case, set mesh properties on model faces of the plane.
      * @param mesh: Simmetrix mesh.
      * @param meshCase: Simmetrix mesh case.
      * @param p: a class to hold the mesh meta data of a plane.
      */ 
     void setMeshOnPlaneFaces(pMesh mesh, pACase meshCase, PlaneMeshMetaData& p);

     /**
      * Function to set one element deep mesh on a model face.
      * @param mesh: Simmetrix mesh.
      * @param meshSize: general mesh size on the model face.
      * @param meshCase: Simmetrix mesh case.
      * @param gf: model face on which one element deep mesh is derired.
      */ 
     void setOneElementDeepMeshOnFace(pMesh mesh, double meshSize, pACase meshCase, pGFace gf); 

     /**
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
