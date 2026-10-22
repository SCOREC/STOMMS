#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "modelingVmec.h"
#include <util.h>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

Model generateSimModelVmec(std::vector <PlaneMetaData> md, const PhysicalGeometry& pg, const VmecData& vm)
{
  // Step 1: Create the model for core region 
  pGModel simModel = generateCoreSimModelVmec(md, vm);

  // Step 2: Add wallcurve to the model.
  generateWallFaceVmec(simModel, md, pg, vm);

  // Step 3: Write the model (.smd) on disk for visualization.
  GM_write(simModel, "simModel.smd", 0, 0);

  // Step 4: Save it as type Model
  Model model;
  model.setSimModel(simModel);

  return model;
}


//From a given plane meta data (Vmec, flux and plane info), generate the model of the core region of stellarator
pGModel generateCoreSimModelVmec(std::vector <PlaneMetaData> md, const VmecData& vm)
{
  // Step 1: Read the data from the vmec object (vm)
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

  // Step 2: Create an object to hold Vmec flux data
  pVmecFlux vf = VmecFlux_create(avmajr, avminr, nsurf, nmode, R.data(), Z.data(), L.data(), iota.data(), psi.data(), xm.data(), xn.data());

  // Step 3: Read number of flux curves (npsi) on each poloidal plane with the respective normalized psi values (psiNorm) from the
  // plane meta data (md). Also, read the number of toroidal planes (nzeta) with the toroidal angles (zetas) of each poloidal plane.
  // Also, read psi values at O-point and last closed flux curve from VMEC file and use them to convert
  // normalized psi to actual psi. 
  std::vector <double> psiNorm = md[0].getPlaneFluxValues();
  std::vector <double> zetas;
  for (int i = 0; i  < md.size(); i++)
  {
    // Step 3.1: The toroidal angles from the plane meta data.
    double toroidalAngle = md[i].getPlaneToroidalAngle();
    zetas.push_back(toroidalAngle);
  }
  const int npsi = psiNorm.size(), nzeta = zetas.size(); 

  // Step 4: Convert normalized psi values to actual psi values. We need read psi values at O-point and last closed 
  // flux curve from VMEC file and use them to convert normalized psi to actual psi.
  double psiAxis = psi[0];
  double psiLCF = psi[nsurf-1];
  std::vector <double> psiVec = convertNormToPsiVector(psiNorm, psiAxis, psiLCF);  

  // Step 5: Set flux curves and planes in the vmec vf object.
  VmecFlux_setFluxes(vf, npsi, 0, psiVec.data()); 
  VmecFlux_setToroidalAngles(vf, nzeta, zetas.data());

  // Step 6: Create model entities from the vmec physics data.
  pGModel simModel = simModelFromVmec(vf, npsi, nzeta, psiVec.data(), zetas.data());

  return simModel;
}

void generateWallFaceVmec(pGModel model, std::vector <PlaneMetaData> md, const PhysicalGeometry& pg, const VmecData& vm)
{
  // Step 0: If physical geometry doesn't have a wall curve, just return and don't do anything.
  if (!pg.hasWallCurves())
    return;

  // Step 1: Iterate over the planes and set wall curve for each plane.
  for (int i = 0; i  < md.size(); i++)
  {
    // Step 1.1: Get the toroidal angle from the plane meta data.
    double toroidalAngle = md[i].getPlaneToroidalAngle();

    // Step 1.2: For each plane, create a wall curve
    WallCurve wallCurve = pg.getWallCurveAtPlane(i);

    // Step 1.3: Get points for each wall curve, and convert them to Cartesian to pass to 
    // Simmetrix API.
    std::vector <Point> wallPoints = wallCurve.getPoints(); 
    std::vector <Point> points = convertFromCylindricalToCartesian(wallPoints, toroidalAngle);

    // Step 1.4: Create Simmetrix wall in terms of Simmetrix curve and edges for the given planes.
    SimmetrixWallCurve simWallCurve(points, model);

    // Step 1.5: Get the last closed flux curve psi value. Can directly get psiVmec[vm.nSurf-1] for 
    // the last closed curve but to make sure we are using correct psiNorm from user given data and
    // not using wrong value from psiVmec. 
    std::vector <double> psiVmec = vm.psi;  // vector of psi values from VMEC
    std::vector <double> psiNorm = md[i].getPlaneFluxValues();  // normalized psi values
    double psiAxis = psiVmec[0];
    double psiLCF = psiVmec[vm.nSurf-1];
    double psi = convertNormToPsi(psiNorm[psiNorm.size()-1], psiAxis, psiLCF);

    // Step 1.6: From the model, get the model edge for last closed curve. 
    pVmecFlux vf = GM_vmec(model);
    pGEdge ge = VmecFlux_poloidalEdge(vf, psi, toroidalAngle);

    // Step 1.7: Using wall edges (outer loop) and last closed egde (inner loop), define
    // a model face and then set its properties.
    pGFace gf = createWallFace(ge, simWallCurve, model);
    GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::NearVacuum), "PhysicsRegion"); 
    GEN_setNativeDoubleAttribute(gf, toroidalAngle, "Zeta");
  }
}

std::vector <Point> convertFromCylindricalToCartesian(const std::vector <Point>& points, double phi)
{
  std::vector <Point> cylindricalPoints;
  for (int i = 0; i < points.size(); i++)
  {
    // Struct Point has R-Z data stored in X-Y(it works fine for 2D planes).
    double r = points[i].x; 
    double x = r*cos(phi);
    double y = r*sin(phi);
    double z = points[i].y;
    Point transformedPoint(x,y,z);
    cylindricalPoints.push_back(transformedPoint);
  }
  return cylindricalPoints;
}

// From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
// generate a Simmetrix model (pGModel model)
pGModel simModelFromVmec(pVmecFlux vf, int npsi, int nzeta, const double *psis, const double *zetas)
{
  std::cout << "=============== Modeling Starts ===============\n";
  // Step 1: Create a new Simmetrix model (pGModel)
  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);
  pGModel model = GM_new(0);

  // Step 2: Set model type (2D or 3D) - Hardcoded for now to test.
  // Add an input parameter in future for it.
  int type = 2; // 2D
  bool model_3d = false;
  if (model_3d)
    type = 3; 

  std::cout << "Model Type: " << (type == 2 ? "2D":"3D") << "\n";
  pGIPart gp = GM_createVmecPart(model, vf, type, prog); 

  // Step 3: Sanity check (Verifying some of the model entities)
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

