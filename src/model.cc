#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "model.h"
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

// Setting model entities from simModel to respective planes
void getPlanes(Model model, std::vector <Plane>& planesContainer, args* a)
{

  // Step 1: Get the Simmetrix model (pGModel) from Model.
  pGModel simModel = model.getSimModel();

  // Step 2: Sort all the model faces and O-point vertices  according to plane number in a map.
  // In map, the key is the plane # and the element is relevant model data on the plane.
  std::map <int, pGVertex> planesAxisMap = sortOPointsByPlanes(simModel, a);
  std::map <int, std::vector<pGFace>> planesFacesMap = sortFacesByPlanes(simModel, a);

  // Step 3: Iterate over the map and assign the data to each plane (using object Plane)
  std::map <int, std::vector<pGFace>>::iterator itr;
  for (itr = planesFacesMap.begin(); itr != planesFacesMap.end(); itr++)
  {
    Plane p;
    
    // Step 3.1: Get the plane number from the map.
    p.planeNumber = (itr->first);

    // Step 3.2: Get the O-point for this planes from the axis map.
    p.oPoint = planesAxisMap[itr->first];  // Set the Opoint on the plane.

    // Step 3.3: Set the flux curves on each plane.
    std::vector <Flux> fluxCurves = setFluxCurvesOnPlanes(simModel, p.planeNumber, a);
    p.fluxCurves = fluxCurves;  // Set flux curves on the plane.

    // Step 3.4: Set the faces on each plane.
    p.modelFaces = itr->second;  // Set the model faces on the plane.

    // Step 3.5: Save the plane in planes container.
    planesContainer.push_back(p);
  }
}

// From the simModel, sort the oPoints by planes.
std::map<int, pGVertex> sortOPointsByPlanes(pGModel model, args* a)
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = a->in.pd.planeInput;

  // Step 2: Look at the Opoint at each plane (zeta value) and set it to the map.
  std::map <int, pGVertex> planesAxisMap;
  for (int i = 0; i < zetas.size(); i++)
  {
    pGVertex axis = VmecFlux_opointVertex(vf, zetas[i]);
    planesAxisMap[i] = axis;
  }
  
  // Return the map between plane number and Opoints.
  return planesAxisMap;
}

// From the simModel, sort the model entities by planes. This results defining
// each plane using its model entities (model faces for now).
std::map<int,std::vector<pGFace>> sortFacesByPlanes(pGModel model, args* a) 
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = a->in.pd.planeInput;

  // Step 2: Iterate over the model faces, read their toroidal angle and compare it to data 
  // in planes vector (zetas) to sort the model faces according to their plane number.
  std::map <int, std::vector<pGFace>> planesFaceMap;
  GFIter fIter = GM_faceIter(model);
  while (pGFace gFace = GFIter_next(fIter))
  {
    double psi0, psi1;
    double zeta;
    pGFace gf = gFace;

    // Setp 2.1: Read the toroidal angle of the model face.
    VmecFlux_poloidalFaceInfo(vf, gf, &psi0, &psi1, &zeta);
    int index = -1;

    // Step 2.2: Compare the angle with the angles provided in input file (read in vector zetas).
    // If similar found, push the model face to respective plane number in planesMap.
    for (int i = 0; i < zetas.size(); i++)
    {
      if (zetas[i] - zeta < 1e-16)
        index = i;
    }
    planesFaceMap[index].push_back(gf);
  }
  GFIter_delete(fIter); 

  // Step 3: Returns the map between plane number and associated vector of model faces.
  return planesFaceMap;
}

// Set all the flux curves on a plane.
std::vector <Flux> setFluxCurvesOnPlanes(pGModel model, int planeNum, args* a)
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the angle
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  double zeta = a->in.pd.planeInput[planeNum];

  // Step 2: Set each flux curve one by one and then push the flux curve to flux 
  // curves container.  Iterate over the fluxMeshSize Map to start with.
  std::vector <Flux> fluxCurvesOnPlane;
  std::map <double, int>::iterator itr;
  for (itr = a->in.fd.fluxMeshSize.begin(); itr != a->in.fd.fluxMeshSize.end(); itr++)
  {
    // Step 2.1: Don't take any action for the O-point
    if (itr->first - 0.0 < 1e-16)
      continue;	 // Ignore the psi value at Opoint

    // Step 2.2: Declare a Flux and assign data to its members.
    Flux f;
    f.planeNumber = planeNum;
    f.psiNormOnFlux = itr->first;

    // Step 2.3: Get the actual psi value from normalized psi and then use the value
    // to retrieve model edge associated to it.
    double psi = convertNormToPsi(itr->first, a->psiAxis, a->psiLCF); 
    pGEdge ge = VmecFlux_poloidalEdge(vf, psi, zeta);

    // Step 2.4: Set the edges in a container and assign remaining member variables of Flux
    f.edgesOnFlux.push_back(ge);  // For now, its a single edge. In future, for open edges we will need to store multiple edges in a container.
    f.numEdgesOnFlux = f.edgesOnFlux.size();
    f.meshVerticesOnFlux = itr->second;

    // Step 2.5: Push the flux curves to a container.
    fluxCurvesOnPlane.push_back(f);
  }
  
  return fluxCurvesOnPlane;
}

