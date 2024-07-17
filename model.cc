#include <netcdf>
#include "ncFile.h"
#include "ncVar.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "model.h"

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
using namespace netCDF;

void generateCoreSimModel(args* a)
{
  double avmajr, avminr,*R, *Z, *L, *xm, *xn;
  int i, nsurf, nmode;

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
  vmecMinor.getVar(&avminr);
  vmecMajor.getVar(&avmajr);
  vmecSurf.getVar(&nsurf);
  vmecMode.getVar(&nmode);

  R = new double[nsurf*nmode];
  Z = new double[nsurf*nmode];
  L = new double[nsurf*nmode];
  xm = new double[nmode];
  xn = new double[nmode];
 
  vmecR.getVar(R);
  vmecZ.getVar(Z);
  vmecL.getVar(L);
  vmecXm.getVar(xm);
  vmecXn.getVar(xn);


  pVmecFlux vf = VmecFlux_create(avmajr, avminr, nsurf, nmode, R,  Z, L, xm, xn);
  delete[] R;
  delete[] Z;
  delete[] L;
  delete[] xm; 
  delete[] xn; 

  const int nrho = 10, nzeta = 5;
  const int rhos[nrho] = {0, 12, 15, 25, 40, 51, 66, 76, 80, 98};
  const double zetas[nzeta] = {0, M_PI/12, M_PI/6, M_PI/4, M_PI/2};
  VmecFlux_setFluxIndices(vf, nrho, rhos);
  VmecFlux_setToroidalAngles(vf, nzeta, zetas);

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



