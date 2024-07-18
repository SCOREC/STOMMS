#include <netcdf>
#include "ncFile.h"
#include "ncVar.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "model.h"
#include <vector>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
using namespace netCDF;



//From a given Vmec File, generate the model of the core region of stellarator
void generateCoreSimModel(args* a)
{
  // Step 1: Read all the variables in vmec file
  NcFile vmecFile(a->vmecFile, NcFile::read); 

  // netCDF Variables. First four hold single values and the rest are arrays of data.
  NcVar vmecMinor, vmecMajor, vmecSurf, vmecMode, vmecR, vmecZ, vmecL, vmecXm, vmecXn;    

  // Step 2: Read required variables from vmec file
  vmecMinor = vmecFile.getVar("Aminor_p");
  vmecMajor = vmecFile.getVar("Rmajor_p");
  vmecSurf = vmecFile.getVar("ns");
  vmecMode = vmecFile.getVar("mnmax");
  vmecR = vmecFile.getVar("rmnc");
  vmecZ = vmecFile.getVar("zmns");
  vmecL = vmecFile.getVar("lmns");
  vmecXm = vmecFile.getVar("xm");
  vmecXn = vmecFile.getVar("xn");


  // Step 3: Assign netCDF variables to local variables and arrays
  double avmajr, avminr;
  int i, nsurf, nmode;

  vmecMinor.getVar(&avminr);
  vmecMajor.getVar(&avmajr);
  vmecSurf.getVar(&nsurf);
  vmecMode.getVar(&nmode);

  std::vector <double> R, Z, L, xm, xn;
  R.resize(nsurf*nmode);
  Z.resize(nsurf*nmode);
  L.resize(nsurf*nmode);
  xm.resize(nmode);
  xn.resize(nmode);

  vmecR.getVar(R.data());
  vmecZ.getVar(Z.data());
  vmecL.getVar(L.data());
  vmecXm.getVar(xm.data());
  vmecXn.getVar(xn.data());

  // Step 4: Create an object to hold Vmec flux data
  pVmecFlux vf = VmecFlux_create(avmajr, avminr, nsurf, nmode, R.data(), Z.data(), L.data(), xm.data(), xn.data());

  // Step 5: Set number of fluxs (nrho) on each poloidal plane and number of poloidal 
  // planes (nzeta) along with their indices (rhos) and toroidal angle (zetas). 
  const int nrho = 10, nzeta = 5;
  const int rhos[nrho] = {0, 12, 15, 25, 40, 51, 66, 76, 80, 98};
  const double zetas[nzeta] = {0, M_PI/12, M_PI/6, M_PI/4, M_PI/2};
  VmecFlux_setFluxIndices(vf, nrho, rhos);
  VmecFlux_setToroidalAngles(vf, nzeta, zetas);

  // Step 6: Create model entities from the vmec physics data.
  pGModel model = GM_new(0);
  pGIPart gp = GM_createVmecPart(model, vf, 2); 
  int rho, rho0, rho1;
  double zeta;
  pGVertex gv = VmecFlux_opointVertex(vf, zetas[2]);
  VmecFlux_opointVertexInfo(vf, gv, &zeta);
  assert(zeta == zetas[2]);
  pGEdge ge = VmecFlux_poloidalEdge(vf, rhos[2], zetas[2]);
  VmecFlux_poloidalEdgeInfo(vf, ge, &rho, &zeta);
  assert(rho == rhos[2] && zeta == zetas[2]);
  pGFace gf = VmecFlux_poloidalFace(vf, rhos[2], zetas[2]);
  VmecFlux_poloidalFaceInfo(vf, gf, &rho0, &rho1, &zeta);
  assert(rho0 == rhos[2] && rho1 == rhos[3] && zeta == zetas[2]);
  GM_write(model, "vmec.smd", 0, 0);
}



