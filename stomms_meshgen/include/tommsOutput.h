#ifndef TOMMSOUTPUT_H
#define TOMMSOUTPUT_H

#include "meshOutput.h"

/**
 * A class to handle Tomms legacy output.
 * Add mesh files (nodes, elements, and flx files).
 */
class TommsOutput{
  public:
    /**
     * TommsOutput constructor takes in StommsMesh and writes nodes, elements, and flx mesh data.
     * @param mesh: Stomms mesh data that holds underlying simmetrix model, mesh, and poloidal plane data.
     */ 
    TommsOutput(const StommsMesh& mesh);

    /**
     * TommsOutput destructor remove the nex data file and wall related ids on the vertices.
     */ 
    ~TommsOutput();
  private:
    // Data Members
    StommsMesh stommsMesh;
    pMesh simMesh;  // Simmetrix mesh - given
    pGModel simModel;  // Simmetrix model - given    
    pMeshNex nex;  // Nex file for mapping - written in the class
    pMeshDataId wallId = MD_newMeshDataId("WallCurve"); // wall-curve flag - 0 or 1
    std::vector <Plane> geometricPlanes;  // Geometric planes data - given
    std::vector <Flux> core, sol, pvt0, pvt1;  // sorted flux curves in each redion
    std::vector <Flux> sortedFluxCurves;   // sorted flux curves on the poloidal plane
    std::map <pGEdge, int> edgesToCurveMap;  // edges-flux Id relationship map
    std::vector <bool> curveUsed;  // a vector needed in sorting of flux curves
 
    // Node Ordering
    int nodeIndx = 1;  // ordering starts at 1 (and not 0 as in simmetrix meshes).
    std::vector <pVertex> nodes;  // a vector of all sorted and ordered nodes.

    /**
     * High level function to write the mesh data to the nodes file.
     */ 
    void writeNodeFile();

    /**
     * Function to find the magnetic axis node and write it to the nodes vector.
     */ 
    void writeAxisNode();

    /**
     * Function to write ordered list of vertices on the flux curves to the nodes vector.
     * Uses a vector of flux curves already sorted according to physics regions and psi values.
     * Open flux curves end points (on the wall) are not written here. Instead they are handled
     * with wall curve vertices.
     */ 
    void writeNodesOnFluxCurves();  

    /**
     * Function to write list of mesh vertices on model edges of the wall curve.
     * Loop over model edges one by one, and uses reverses classification information.
     */ 
    void writeNodesOnWallCurve();

    /**
     * Function to write any nodes (unstructured) that haven't been written yet by the 
     * above three functions. Simply loops over the all mesh vertices, and if the node
     * is not in the nodes vector yet, put it in the vector.
     */ 
    void writeUnstructuredNodes();

    /**
     * Function to write information stored in nodes vector to an output file.
     */ 
    void writeNodeDataToFile(); 

    /**
     * Function to write the connectivity informaton of elements to the elements file.
     * Write the three indices of the vertices on each element. These indices are already
     * ordered and their mapping is in nex file.
     *
     */ 
    void writeElementsFile();

    /**
     * Function to set model curves to the specific physics region. It sorts curves according to 
     * psi values in each physics region and set them in a vector specific to that physics region.
     */ 
    void setFluxCurvesOnPhysicsRegions(); 

    /**
     * Given the tags of model faces on the specific region, this functions adds all the flux curves (sorted by psi) 
     * bounding these model faces to the curves vector.
     * @param faceTags: Simmetrix tags of the model faces defining a physics region.
     * @param privateId: If private region, set private region id (0 or 1), else -1.
     * @param curves: a  vector to save the sorted list of flux curves on specific physics region (defined by faceTags).
     */ 
    void addCurvesToRegion(const std::vector <int>& faceTags, int privateId, std::vector <Flux>& curves); 

    /**
     * Function to check if a mesh vertex is classified on the wall curve or not.
     * @param v: mesh vertex to be checked.
     * @return true if mesh vertex v is classified on the wall curve, else fale.
     */ 
    bool isOnWall(pVertex v);

    /**
     * Function to check if a model vertex is classified on the wall curve or not.
     * @param gv: model vertex to be checked.
     * @return true if model vertex gv is classified on the wall curve, else fale.
     */ 
    bool isOnWall(pGVertex gv);

    /**
     * Function to check if a model edge is classified on the wall curve or not.
     * @param ge: model edge to be checked.
     * @return true if model edge ge is classified on the wall curve, else fale.
     */ 
    bool isOnWall(pGEdge ge);
};


#endif
