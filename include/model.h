#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include "MeshSim.h"
#include "SimAdvModel.h"
#include "SimModel.h"
#include "SimUtil.h"
#include "input.h"

class Flux{
  public:
    int planeNumber;
    double psiNormOnFlux;
    std::vector <pGEdge> edgesOnFlux;
    int numEdgesOnFlux;
    int meshVerticesOnFlux;  // Number of vertices desired on flux curves
    std::vector <Pt> pts;    // The field following points on each flux curve.
};

class Plane{
  public:
    std::vector <pGFace> modelFaces;
    std::vector <Flux> fluxCurves;
    pGVertex oPoint;
    int planeNumber;
};

/*
 * From a given Vmec File, generate the model of the core region of stellarator
 * args* a (in): input parameters. 
 * returns pGModel;
*/
pGModel generateCoreSimModel(args* a);

/* 
 * From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
 * generate a Simmetrix model (pGModel model).
 * pVmecFlux vf (in): flux data on all the planes.
 * int npsi (in): number of flux curves on each plane.
 * int nzeta (in): number of poloidal planes.
 * int *psis (in): an input array holding flux normalized psi values.
 * double *zetas:an input array holding toroidal angles of the poloidal planes.
 * returns pGModel.
*/
pGModel simModelFromVmec(pVmecFlux vf, int npsi, int nzeta, const double *psis, const double *zetas);

/*
 * Setting model entities from simModel to respective planes.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * std::vector <plane> planesContainer (out): the data for each plane  as object Plane is written to this container.
 * args* a (in): input parameters.
*/
void getPlanes(pGModel model, std::vector <Plane>& planesContainer, args* a);

/*
 * From the simModel, sort the O-point model vertices by planes.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and corresponding O-point model vertex.
*/
std::map<int, pGVertex> sortOPointsByPlanes(pGModel model, args* a);

/*
 * From the simModel, sort the model faces by planes. 
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and a vector containing all the model faces on that plane.
*/
std::map<int,std::vector<pGFace>> sortFacesByPlanes(pGModel model, args* a);

/*
 * set all the flux curves on a plane.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * int planeNum (in): the plane number on which flux curves will be set.
 * args* a (in): input parameters.
 * returns a vector of flux curves (type Flux).
*/
std::vector<Flux> setFluxCurvesOnPlanes(pGModel model, int planeNum, args* a);

/*
 * Add documentation once its working.
 */
void adjustModelVertexOnEdge(Flux &f);
#endif
