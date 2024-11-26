#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "modeling.h"
#include <util.h>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

//From a given Vmec File, generate the model of the core region of stellarator
Model generateCoreSimModel(args* a)
{
  // Step 1: Read the data from the vmec object (vm)
  double avmajr = a->in.vm.majorR;
  double avminr = a->in.vm.minorR;
  int nsurf = a->in.vm.nSurf;
  int nmode = a->in.vm.nMode;
  std::vector <double> R = a->in.vm.R;
  std::vector <double> Z = a->in.vm.Z;
  std::vector <double> L = a->in.vm.L;
  std::vector <double> psi = a->in.vm.psi;
  std::vector <double> xm = a->in.vm.xm;
  std::vector <double> xn = a->in.vm.xn;

  // Step 2: Create an object to hold Vmec flux data
  pVmecFlux vf = VmecFlux_create(avmajr, avminr, nsurf, nmode, R.data(), Z.data(), L.data(), psi.data(), xm.data(), xn.data());

  // Step 3: Read number of flux curves (npsi) on each poloidal plane with the respective normalized psi values (psiNorm) from the
  // fluxFile. Also, rad the number of toroidal planes (nzeta) with the toroidal angles (zetas) of each poloidal plane.
  // Also, read psi values at O-point and last closed flux curve from VMEC file and use them to convert
  // normalized psi to actual psi. 
 
  std::vector <double> psiNorm = a->in.fd.fluxInput;
  std::vector <double> zetas = a->in.pd.planeInput;
  const int npsi = psiNorm.size(), nzeta = zetas.size(); 

  // Step 4: Convert normalized psi values to actual psi values. We need read psi values at O-point and last closed 
  // flux curve from VMEC file and use them to convert normalized psi to actual psi.
  double psiAxis = psi[0];
  double psiLCF = psi[nsurf-1];

  // Set these values of a for global use.
  a->psiAxis = psiAxis;
  a->psiLCF = psiLCF;
  std::vector <double> psiVec = convertNormToPsiVector(psiNorm, psiAxis, psiLCF);  

  // Step 5: Set flux curves and planes in the vmec vf object.
  VmecFlux_setFluxes(vf, npsi, 0, psiVec.data()); 
  VmecFlux_setToroidalAngles(vf, nzeta, zetas.data());

  // Step 6: Create model entities from the vmec physics data.
  pGModel simModel = simModelFromVmec(vf, npsi, nzeta, psiVec.data(), zetas.data());

  // Step 7: Write the model (.smd) on disk for visualization.
  GM_write(simModel, "vmec.smd", 0, 0);

  // Step 8: Save it as type Model
  Model model(simModel);

  return model;
}

// From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
// generate a Simmetrix model (pGModel model)
pGModel simModelFromVmec(pVmecFlux vf, int npsi, int nzeta, const double *psis, const double *zetas)
{
  std::cout << " ============ Modeling Starts ============\n";
  // Step 1: Declare and create Model from vf data
  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);
  pGModel model = GM_new(0);
  pGIPart gp = GM_createVmecPart(model, vf, 2, prog); 

  // Step 2: Sanity check (Verifying some of the model entities)
  double psi, psi0, psi1;
  double zeta;
  pGVertex gv = VmecFlux_opointVertex(vf, zetas[2]);
  VmecFlux_opointVertexInfo(vf, gv, &zeta);
  assert(zeta == zetas[2]);
  pGEdge ge = VmecFlux_poloidalEdge(vf, psis[2], zetas[2]);
  VmecFlux_poloidalEdgeInfo(vf, ge, &psi, &zeta);
  assert(psi == psis[2] && zeta == zetas[2]);
  pGFace gf = VmecFlux_poloidalFace(vf, psis[2], zetas[2]);
  VmecFlux_poloidalFaceInfo(vf, gf, &psi0, &psi1, &zeta);
  assert(psi0 == psis[2] && psi1 == psis[3] && zeta == zetas[2]);

  Progress_delete(prog);
  return model;
}

