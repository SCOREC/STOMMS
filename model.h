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
 * pGModel &globalModel (out): To save model for the global use.
 * args* a (in): Input parameters. 
*/
void generateCoreSimModel(pGModel &globalModel, args* a);

/* From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
 * generate a Simmetrix model (pGModel model).
 * pGModel &model (out): pGModel model object passed to function to store model information. 
 * pVmecFlux vf (in): flux data on all the planes.
 * int nrho (in): number of flux curves on each plane.
 * int nzeta (in): number of poloidal planes.
 * int *rhos (in): an input array holding flux indices.
 * double *zetas:an input array holding toroidal angles of the poloidal planes.
*/
void simModelFromVmec(pGModel &model,pVmecFlux vf, const int nrho, const int nzeta, const int *rhos, const  double *zetas);

#endif
