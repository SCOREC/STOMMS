#ifndef MODELDATA_H
#define MODELDATA_H

#include "magneticGeometry.h"
#include "modelTopology.h"
#include "modelMetaData.h"
#include "modeling.h"
#include "input.h"
#include <util.h>
#include <map>


// A class to contain the information fo a flux curve. 
class Flux{
  public:
    int planeNumber;  // plane on which flux curve lies.
    double psiNormOnFlux;  // normalized psi value of flux curve
    std::vector <pGEdge> edgesOnFlux;  // vector of model edges on the flux curve.
    int meshVerticesOnFlux;  // Number of vertices desired on flux curves
};

class Plane{
  public:
    std::vector <pGFace> modelFaces;  // vector of model faces on the poloidal plane.
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    pGVertex oPoint;  // model vertex on O-Point
    int planeNumber;  // plane number starting from 0 to numPlanes-1
};

// The class StommsModel contains the model information after the creation of model from the meta data and
// also model information stored for each plane.
class StommsModel{
  public:
    /*
     * Constructor gets the model meta data and uses it to setup StommsModel.
     * const ModelMetaData& md (in): Takes the object of class ModelMetaData as input.
     */   
    StommsModel(const ModelMetaData& md);

    /*
     * Setting model entities from Simmetrix Model (pGModel) to respective planes.
     */
    void setPlanes();

    /*
     * Function to return the vector containing all the planes with their geometric data.
     */ 
    const std::vector <Plane>& getPlanes();

    /*
     * Function to return the underlying model in StommsModel.
     */ 
    const Model& getModel();

    /*
     * Function to return ModelMetaData stored in StommsModel.
     */ 
     const ModelMetaData& getModelMetaData();
  private:
    ModelMetaData modelMetaData;  // Object of class ModelMetaData holding all the model meta data.
    std::vector <PlaneMetaData> planesContainer;  // a vector to hold all planes with their meta data.
    std::vector <Plane> planes;  // a vector to hold all the planes with their geometric data.
    Model model;  // underlying geometric model in StommsModel.
};

/*
 * From the Model, sort the O-point model vertices by planes.
 * Model model (in): the model created from modeling step.
 * std::vector <double> planeAngles (in): vector containing the toroidal angles of all the planes.
 * returns a map between plane number and corresponding O-point model vertex.
*/
std::map<int, pGVertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles); 

/*
 * From the Model, sort the model faces by planes. 
 * Model model (in): the model created from modeling step.
 * std::vector <double> planeAngles (in): vector containing the toroidal angles of all the planes.
 * returns a map between plane number and a vector containing all the model faces on that plane.
*/
std::map<int,std::vector<pGFace>> sortFacesByPlanes(Model m, std::vector <double> planeAngles); 

/*
 * set all the flux curves on a plane.
 * Model model (in): the model created from modeling step.
 * int planeNum (in): the plane number on which flux curves will be set.
 * std::vector <PlaneMetaData> md (in): vector holding  all planes with their meta data.
 * returns a vector of flux curves (type Flux).
*/
std::vector<Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md);

#endif
