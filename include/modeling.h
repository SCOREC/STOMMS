#ifndef MODELING_H
#define MODELING_H

#include <map>
#include "input.h"
#include "modelTopology.h"

/*
 * From a given Vmec File, generate the model of the core region of stellarator
 * args* a (in): input parameters. 
 * returns pGModel;
*/
Model generateCoreSimModel(args* a);

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

#endif
