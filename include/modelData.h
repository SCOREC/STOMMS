#ifndef MODELDATA_H
#define MODELDATA_H

#include "magneticGeometry.h"
#include "modelTopology.h"
#include "modelMetaData.h"
#include "input.h"
#include <util.h>
#include <map>

class Flux{
  public:
    int planeNumber;
    double psiNormOnFlux;
    std::vector <pGEdge> edgesOnFlux;
    int numEdgesOnFlux;
    int meshVerticesOnFlux;  // Number of vertices desired on flux curves
};

class Plane{
  public:
    std::vector <pGFace> modelFaces;
    std::vector <Flux> fluxCurves;
    pGVertex oPoint;
    int planeNumber;
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
