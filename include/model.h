#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include "MeshSim.h"
#include "SimAdvModel.h"
#include "SimModel.h"
#include "SimUtil.h"
#include "input.h"

class Plane{
  public:
    std::vector <pGFace> modelFaces;
    std::vector <pGEdge> modelEdges;
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
 * Read the flux input file and check its validity.
 * args* a (in): input parameters.
 * returns a vector containing the normalized psi values of desired flux curves.
*/
std::vector<double> readFluxFile(args* a);

/*
 * Convert the normalized psi values to actual psi values. Takes normalized psi values
 * in a vector and return a vector of actual psi values.
 * std::vector <double> normPsi (in): vector of normalized psi values.
 * double psiAxis (in): actual psi value at the axis (O-point).
 * double psiLCF (in): actual psi value at the last closed curve.
 * returns a vector of actual psi values.
*/ 
std::vector <double> convertNormToPsi(std::vector <double> normPsi, double psiAxis, double psiLCF);

/*
 * Read the planes input files and check its validity.
 * converts the given angles in degrees to radians.
 * args* a (in): input parameters.
 * returns a vector containing the toroidal angles of the desired poloidal planes.
*/
std::vector<double> readPlaneFile(args* a);

/*
 * Setting model entities from simModel to respective planes.
 * pGModel model (in): the Simmetrix model created from modeling step.
 * std::vector <plane> planesContainer (out): the data for each plane  as object Plane is written to this container.
 * args* a (in): input parameters.
*/
void getPlanes(pGModel model, std::vector <Plane>& planesContainer, args* a);

/*
 * From the simModel, sort the model entities by planes. This results defining
 * each plane using its model entities (model faces for now).
 * pGModel model (in): the Simmetrix model created from modeling step.
 * args* a (in): input parameters.
 * returns a map between plane number and a vector containing all the model faces on that plane.
*/
std::map<int,std::vector<pGFace>> sortFacesbyPlanes(pGModel model, args* a);

#endif
