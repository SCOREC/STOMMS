#include "modelData.h"

// From the simModel, sort the oPoints by planes.
std::map<int, pGVertex> sortOPointsByPlanes(pGModel model, const args& a)
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = a.in.pd.planeInput;

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
std::map<int,std::vector<pGFace>> sortFacesByPlanes(pGModel model, const args& a) 
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = a.in.pd.planeInput;

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
std::vector <Flux> setFluxCurvesOnPlanes(pGModel model, int planeNum, const args& a)
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the angle
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  double zeta = a.in.pd.planeInput[planeNum];

  // Step 2: Set each flux curve one by one and then push the flux curve to flux 
  // curves container.  Iterate over the fluxMeshSize Map to start with.
  std::vector <Flux> fluxCurvesOnPlane;
  std::map <double, int>::iterator itr;
  for( const auto &itr : a.in.fd.fluxMeshSize)
  {
    // Step 2.1: Don't take any action for the O-point
    if (itr.first - 0.0 < 1e-16)
      continue;  // Ignore the psi value at Opoint

    // Step 2.2: Declare a Flux and assign data to its members.
    Flux f;
    f.planeNumber = planeNum;
    f.psiNormOnFlux = itr.first;

    // Step 2.3: Get the actual psi value from normalized psi and then use the value
    // to retrieve model edge associated to it.
    double psi = convertNormToPsi(itr.first, a.psiAxis, a.psiLCF); 
    pGEdge ge = VmecFlux_poloidalEdge(vf, psi, zeta);

    // Step 2.4: Set the edges in a container and assign remaining member variables of Flux
    f.edgesOnFlux.push_back(ge);  // For now, its a single edge. In future, for open edges we will need to store multiple edges in a container.
    f.numEdgesOnFlux = f.edgesOnFlux.size();
    f.meshVerticesOnFlux = itr.second;

    // Step 2.5: Push the flux curves to a container.
    fluxCurvesOnPlane.push_back(f);
  }
  
  return fluxCurvesOnPlane;
}

