#ifndef MODELDATAVMEC_H
#define MODELDATAVMEC_H 

#include "modelingVmec.h"
#include "physicsAttributes.h"
#include "plane.h"

/**
 * Class to create a geometry based on model meta data, and vmec info.
 * Also, properties specific to vmec, and stellarators are set in 
 * this class.
 */
class ModelVmec{
  public:
    /**
     * Constructor to create an object of ModelVmec.
     */ 
    ModelVmec(){};

    /**
     * Constructor to create model based on metadata and magnetic field information.
     * @param md: model metadata information to setup geometry.
     * @param vm: struct containing all the necessary vmec information for modeling.
     */ 
    ModelVmec(const ModelMetaData& md, const VmecData& vm);

    /**
     * Function to get model associated with vmec geometry. 
     */ 
    const Model& getModel() const;

    /**
     * Function to get all the geometric information on individual planes.
     */ 
    const std::vector <Plane>& getPlanes() const;
  private:
    ModelMetaData modelMetaData;  // Object of class ModelMetaData holding all the model meta data.
    std::vector <PlaneMetaData> planesContainer;  // a vector to hold all planes with their meta data.
    Model model;  // Model class. It has underlying Simmetrix model too.
    std::vector <Plane> planes;  // a vector to hold all the planes with their geometric data.
    VmecData vmec;  // holds all the information needed for vmec geometry construction.

  /**
   * Setting model entities from Simmetrix Model (pGModel) to respective planes.
   */
   void setPlanes();

  /**
   * From the Model, sort the O-point model vertices by planes.
   * @param model: the model created from modeling step.
   * @param planeAngles: vector containing the toroidal angles of all the planes.
   * @return a map between plane number and corresponding O-point model vertex.
   */
  std::map<int, Vertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles); 

  /**
   * From the Model, sort the model faces by planes. 
   * @param model: the model created from modeling step.
   * @param planeAngles: vector containing the toroidal angles of all the planes.
   * @return a map between plane number and a vector containing all the model faces on that plane.
   */
  std::map<int,std::vector<Face>> sortFacesByPlanes(Model m, std::vector <double> planeAngles); 

  /**
   * set all the flux curves on a plane.
   * @param model: the model created from modeling step.
   * @param planeNum: the plane number on which flux curves will be set.
   * @param md: vector holding  all planes with their meta data.
   * @return a vector of flux curves (type Flux).
   */
  std::vector<Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md);

  /**
   * Given set of planes, set the mesh vertices on the flux curves on all plane.
   */
  void setMeshVerticesOnPlanes();  

  /**
   * Given a single plane, set mesh vertices on the flux curves that belong to the plane.
   * @param planeIndex: index of planes in the vector of planes.
   */  
  void setMeshVerticesOnPlane(int planeIndex);
 
  /**
   * Function to set physics type on model faces.
   */ 
  void classifyModelFaces();

};

#endif
