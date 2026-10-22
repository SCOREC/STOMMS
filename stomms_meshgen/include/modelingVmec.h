#ifndef MODELINGVMEC_H
#define MODELINGVMEC_H

#include <map>
#include "input.h"
#include "vmecData.h"
#include "magneticGeometry.h"
#include "modeling.h"

/**
 * From given Vmec data and meta data, generate the model of the stellarator.
 * @param mD: a vector of planes with their meta data.
 * @param pg: Information related to phsyical geometry (wall curve for now).
 * @param vm: vmec class containing all the vmec data.
 */
Model generateSimModelVmec(std::vector <PlaneMetaData> md, const PhysicalGeometry& pg, const VmecData& vm);

/**
 * Given a pGModel and wall data, add wall curves and model face to the model.
 * @param mD: a vector of planes with their meta data.
 * @param pg: Information related to phsyical geometry (wall curve for now).
 * @param vm: vmec class containing all the vmec data.
 */
void generateWallFaceVmec(pGModel model, std::vector <PlaneMetaData> md, const PhysicalGeometry& pg, const VmecData& vm);

/**
 * Given a vector of points in RZ coordinate, and an input toroidal angle phi, convert the points to Cartesian coordinates.
 * @param points: a vector of RZ points (in Cylindrical coordinates).
 * @param phi: toroidal angle phi.
 */
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
