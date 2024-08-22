#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "MeshSim.h"
#include "SimAdvModel.h"
#include "SimModel.h"
#include "SimUtil.h"
#include "input.h"


/*
 * From a given Vmec File, generate the model of the core region of stellarator
 * args* a (in): Input parameters. 
 * returns pGModel;
*/
pGModel generateCoreSimModel(args* a);

/* 
 * From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
 * generate a Simmetrix model (pGModel model).
 * pVmecFlux vf (in): flux data on all the planes.
 * int nrho (in): number of flux curves on each plane.
 * int nzeta (in): number of poloidal planes.
 * int *rhos (in): an input array holding flux indices.
 * double *zetas:an input array holding toroidal angles of the poloidal planes.
 * returns pGModel.
*/
pGModel simModelFromVmec(pVmecFlux vf, int nrho, int nzeta, const int *rhos, const  double *zetas);

/*
 * Read the flux input file and check its validity.
 * int nsurf (in): The number of flux surfaces from the VMEC file.
 * args* a (in): Input parameters.
*/
std::vector<int> readFluxFile(int nsurf, args* a);

/*
 * Read the planes input files and check its validity.
 * converts the given angles in degrees to radians.
 * args* a (in): Input parameters.
*/
std::vector<double> readPlaneFile(args* a);

#endif
