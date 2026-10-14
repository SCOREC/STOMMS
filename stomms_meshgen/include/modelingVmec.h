#ifndef MODELINGVMEC_H
#define MODELINGVMEC_H

#include <map>
#include "input.h"
#include "vmecData.h"
#include "magneticGeometry.h"
#include "modeling.h"

/**
 * From a given Vmec File, generate the model of the core region of stellarator.
 *
 */
Model generateSimModelVmec(std::vector <PlaneMetaData> md, const VmecData& vm);

void generateWallFaceVmec(pGModel model, std::vector <PlaneMetaData> md, const VmecData& vm);
std::vector <Point> convertFromCylindricalToCartesian(const std::vector <Point>& points, double phi);

/**
 * From a given Vmec File, generate the model of the core region of stellarator
 * @param mD: a vector of planes with their meta data. 
 * @param vm: an object of vmec data class.
 * @return an object of Simmetrix model pGModel.
*/
pGModel generateCoreSimModelVmec(std::vector <PlaneMetaData> mD, const VmecData& vm);

/**
 * From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
 * generate a Simmetrix model (pGModel model).
 * @param vf: flux data on all the planes.
 * @param npsi: number of flux curves on each plane.
 * @param nzeta: number of poloidal planes.
 * @param psis: an input array holding flux normalized psi values.
 * @param zetas:an input array holding toroidal angles of the poloidal planes.
 * @return pGModel(Simmetrix Model).
*/
pGModel simModelFromVmec(pVmecFlux vf, int npsi, int nzeta, const double *psis, const double *zetas);

#endif
