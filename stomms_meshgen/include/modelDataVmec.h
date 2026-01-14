#ifndef MODELDATAVMEC_H
#define MODELDATAVMEC_H 

#include "modelingVmec.h"

class ModelVmec{
  public:
    ModelVmec(){};
    ModelVmec(const ModelMetaData& md, const VmecData& vm);
    const Model& getModel() const;
    const std::vector <Plane>& getPlanes() const;
  private:
    ModelMetaData modelMetaData;  // Object of class ModelMetaData holding all the model meta data.
    std::vector <PlaneMetaData> planesContainer;  // a vector to hold all planes with their meta data.
    Model model;
    std::vector <Plane> planes;  // a vector to hold all the planes with their geometric data.
    VmecData vmec; 
  /*
   * Setting model entities from Simmetrix Model (pGModel) to respective planes.
  */
   void setPlanes();

  /*
   * From the Model, sort the O-point model vertices by planes.
   * Model model (in): the model created from modeling step.
   * std::vector <double> planeAngles (in): vector containing the toroidal angles of all the planes.
   * returns a map between plane number and corresponding O-point model vertex.
  */
  std::map<int, Vertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles); 

  /*
   * From the Model, sort the model faces by planes. 
   * Model model (in): the model created from modeling step.
   * std::vector <double> planeAngles (in): vector containing the toroidal angles of all the planes.
   * returns a map between plane number and a vector containing all the model faces on that plane.
  */
  std::map<int,std::vector<Face>> sortFacesByPlanes(Model m, std::vector <double> planeAngles); 

  /*
   * set all the flux curves on a plane.
   * Model model (in): the model created from modeling step.
   * int planeNum (in): the plane number on which flux curves will be set.
   * std::vector <PlaneMetaData> md (in): vector holding  all planes with their meta data.
   * returns a vector of flux curves (type Flux).
  */
  std::vector<Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md);

};

#endif
