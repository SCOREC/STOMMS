#ifndef MODELDATA_H
#define MODELDATA_H

#include "modelTopology.h"
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
 * Setting model entities from simModel to respective planes.
 * pGModel model (in): the model created from modeling step.
 * std::vector <plane> planesContainer (out): the data for each plane  as object Plane is written to this container.
 * args* a (in): input parameters.
*/
void getPlanes(Model model, std::vector <Plane>& planesContainer, const args& a); 

/*
 * From the simModel, sort the O-point model vertices by planes.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and corresponding O-point model vertex.
*/
std::map<int, pGVertex> sortOPointsByPlanes(pGModel model, const args& a); 

/*
 * From the simModel, sort the model faces by planes. 
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and a vector containing all the model faces on that plane.
*/
std::map<int,std::vector<pGFace>> sortFacesByPlanes(pGModel model, const args& a); 

/*
 * set all the flux curves on a plane.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * int planeNum (in): the plane number on which flux curves will be set.
 * args* a (in): input parameters.
 * returns a vector of flux curves (type Flux).
*/
std::vector<Flux> setFluxCurvesOnPlanes(pGModel model, int planeNum, const args& a);

#endif
