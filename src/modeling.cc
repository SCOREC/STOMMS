#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "modeling.h"
#include <util.h>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

//From a given plane meta data (Vmec, flux and plane info), generate the model of the core region of stellarator
Model generateCoreSimModel(std::vector <PlaneMetaData> md)
{
  // Step 1: Retrieve the data (magnetic geometric) from the MetaData
  // and read vmecData from it.
  MagneticGeometry mg = md[0].getMagneticGeometry();
  VmecData vm = mg.getVmecData();  

  // Step 2: Read the data from the vmec object (vm)
  double avmajr = vm.majorR;
  double avminr = vm.minorR;
  int nsurf = vm.nSurf;
  int nmode = vm.nMode;
  std::vector <double> R = vm.R;
  std::vector <double> Z = vm.Z;
  std::vector <double> L = vm.L;
  std::vector <double> iota = vm.iota;
  std::vector <double> psi = vm.psi;
  std::vector <double> xm = vm.xm;
  std::vector <double> xn = vm.xn;

  // Step 3: Create an object to hold Vmec flux data
  pVmecFlux vf = VmecFlux_create(avmajr, avminr, nsurf, nmode, R.data(), Z.data(), L.data(), iota.data(), psi.data(), xm.data(), xn.data());

  // Step 4: Read number of flux curves (npsi) on each poloidal plane with the respective normalized psi values (psiNorm) from the
  // plane meta data (md). Also, read the number of toroidal planes (nzeta) with the toroidal angles (zetas) of each poloidal plane.
  // Also, read psi values at O-point and last closed flux curve from VMEC file and use them to convert
  // normalized psi to actual psi. 
  std::vector <double> psiNorm = md[0].getPlaneFluxValues();
  std::vector <double> zetas;
  for (int i = 0; i  < md.size(); i++)
  {
    // Step 4.1: The toroidal angles from the plane meta data.
    double toroidalAngle = md[i].getPlaneToroidalAngle();
    zetas.push_back(toroidalAngle);
  }
  const int npsi = psiNorm.size(), nzeta = zetas.size(); 

  // Step 5: Convert normalized psi values to actual psi values. We need read psi values at O-point and last closed 
  // flux curve from VMEC file and use them to convert normalized psi to actual psi.
  double psiAxis = psi[0];
  double psiLCF = psi[nsurf-1];
  std::vector <double> psiVec = convertNormToPsiVector(psiNorm, psiAxis, psiLCF);  

  // Step 6: Set flux curves and planes in the vmec vf object.
  VmecFlux_setFluxes(vf, npsi, 0, psiVec.data()); 
  VmecFlux_setToroidalAngles(vf, nzeta, zetas.data());

  // Step 7: Create model entities from the vmec physics data.
  pGModel simModel = simModelFromVmec(vf, npsi, nzeta, psiVec.data(), zetas.data());

  // Step 8: Write the model (.smd) on disk for visualization.
  GM_write(simModel, "vmec.smd", 0, 0);

  // Step 9: Save it as type Model
  Model model;
  model.setSimModel(simModel);

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
  // This check assumes minimum 3 poloidal planes. Only check it
  // if nzeta > 2.
  if (nzeta > 2)
  {
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
  }

  Progress_delete(prog);
  return model;
}

