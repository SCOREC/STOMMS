#include "modelData.h"

StommsModel::StommsModel(const std::vector <PlaneMetaData> pD):planesContainer(pD)
{
  model = generateCoreSimModel(planesContainer);
  setPlanes();
}

void StommsModel::setPlanes()
{
  // Step 1: Get the Simmetrix model (pGModel) from Model.
  pGModel simModel = model.getSimModel();
  std::vector <double> planeAngles;
  for (int i = 0; i < planesContainer.size(); i++)
  {
    double angle = planesContainer[i].getPlaneToroidalAngle();
    planeAngles.push_back(angle);
  }

  // Step 2: Sort all the model faces and O-point vertices  according to plane number in a map.
  // In map, the key is the plane # and the element is relevant model data on the plane.
  std::map <int, pGVertex> planesAxisMap = sortOPointsByPlanes(model, planeAngles); 
  std::map <int, std::vector<pGFace>> planesFacesMap = sortFacesByPlanes(model, planeAngles); 

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
    std::vector <Flux> fluxCurves = setFluxCurvesOnPlanes(model, p.planeNumber, planesContainer); 
    p.fluxCurves = fluxCurves;  // Set flux curves on the plane.

    // Step 3.4: Set the faces on each plane.
    p.modelFaces = itr->second;  // Set the model faces on the plane.

    // Step 3.5: Save the plane in planes container.
    planes.push_back(p);
  }
}

const Model& StommsModel::getModel()
{
  return model;
}

const std::vector <Plane>& StommsModel::getPlanes()
{
  return planes;
}

// From the simModel, sort the oPoints by planes.
std::map<int, pGVertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles)
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = planeAngles;

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
std::map<int,std::vector<pGFace>> sortFacesByPlanes(Model m, std::vector <double> planeAngles) 
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = planeAngles;

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
std::vector <Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md)
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the angle
  // information from the input plane file.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  double zeta = md[planeNum].getPlaneToroidalAngle();
  MagneticGeometry mg = md[planeNum].getMagneticGeometry();
  double psiAxis = mg.getPsiAxis();
  double psiLCF = mg.getPsiLCFS();

  // Step 2: Set each flux curve one by one and then push the flux curve to flux 
  // curves container.  Iterate over the fluxMeshSize Map to start with.
  std::vector <Flux> fluxCurvesOnPlane;
  std::vector <double> psiValues = md[planeNum].getPlaneFluxValues();
  std::vector <int> numVerticesOnFlux = md[planeNum].getPlaneFluxSizes();
  for( int i = 0; i < numVerticesOnFlux.size(); i++)
  {
    // Step 2.1: Don't take any action for the O-point
    if (numVerticesOnFlux[i] == 1)
      continue;  // Ignore the psi value at Opoint

    // Step 2.2: Declare a Flux and assign data to its members.
    Flux f;
    f.planeNumber = planeNum;
    f.psiNormOnFlux = psiValues[i];

    // Step 2.3: Get the actual psi value from normalized psi and then use the value
    // to retrieve model edge associated to it.
    double psi = convertNormToPsi(psiValues[i], psiAxis, psiLCF); 
    pGEdge ge = VmecFlux_poloidalEdge(vf, psi, zeta);

    // Step 2.4: Set the edges in a container and assign remaining member variables of Flux
    f.edgesOnFlux.push_back(ge);  // For now, its a single edge. In future, for open edges we will need to store multiple edges in a container.
    f.meshVerticesOnFlux = numVerticesOnFlux[i];   

    // Step 2.5: Push the flux curves to a container.
    fluxCurvesOnPlane.push_back(f);
  }
  
  return fluxCurvesOnPlane;
}

