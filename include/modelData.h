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
    StommsModel(const std::vector <PlaneMetaData> pD);
    void setPlanes();
    const std::vector <Plane>& getPlanes();
    const Model& getModel();
  private:
    std::vector <PlaneMetaData> planesContainer;
    std::vector <Plane> planes;
    Model model;
};

/*
 * From the simModel, sort the O-point model vertices by planes.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and corresponding O-point model vertex.
*/
std::map<int, pGVertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles); 

/*
 * From the simModel, sort the model faces by planes. 
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and a vector containing all the model faces on that plane.
*/
std::map<int,std::vector<pGFace>> sortFacesByPlanes(Model m, std::vector <double> planeAngles); 

/*
 * set all the flux curves on a plane.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * int planeNum (in): the plane number on which flux curves will be set.
 * args* a (in): input parameters.
 * returns a vector of flux curves (type Flux).
*/
std::vector<Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md);

#endif
