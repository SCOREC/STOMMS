#include "modelData.h"

// Constructor gets the model meta data and uses it to setup StommsModel.
StommsModel::StommsModel(const ModelMetaData& md):modelMetaData(md)
{
  // Step 1: Get the vector of planes with their meta data from ModelMetadata.
  planesContainer = modelMetaData.getPlanesContainer(); 

  // Step 2: Generate the model using planes meta data.
  model = generateCoreSimModel(planesContainer);

  // Step 3: Steup the poloidal planes with their model enteties using 
  // the model information.
  setPlanes();
}

// Setting model entities from Simmetrix Model (pGModel) to respective planes.
void StommsModel::setPlanes()
{
  // Step 1: Get the Simmetrix model (pGModel) from Model.
  pGModel simModel = model.getSimModel();

  // Step 2: Get the planes angel data from the modelMetaData.
  std::vector <double> planeAngles = modelMetaData.getToroidalAnglesMetaData();

  // Step 3: Sort all the model faces and O-point vertices  according to plane number in a map.
  // In map, the key is the plane # and the element is relevant model data on the plane.
  std::map <int, Vertex> planesAxisMap = sortOPointsByPlanes(model, planeAngles); 
  std::map <int, std::vector<Face>> planesFacesMap = sortFacesByPlanes(model, planeAngles); 

  // Step 4: Iterate over the map and assign the data to each plane (using object Plane)
  std::map <int, std::vector<Face>>::iterator itr;
  for (itr = planesFacesMap.begin(); itr != planesFacesMap.end(); itr++)
  {
    Plane p;

    // Step 4.1: Get the plane number from the map.
    p.planeNumber = (itr->first);

    // Step 4.2: Get the O-point for this planes from the axis map.
    p.oPoint = planesAxisMap[itr->first];  // Set the Opoint on the plane.

    // Step 4.3: Set the flux curves on each plane.
    std::vector <Flux> fluxCurves = setFluxCurvesOnPlanes(model, p.planeNumber, planesContainer); 
    p.fluxCurves = fluxCurves;  // Set flux curves on the plane.

    // Step 4.4: Set the faces on each plane.
    p.modelFaces = itr->second;  // Set the model faces on the plane.

    // Step 4.5: Save the plane in planes container.
    planes.push_back(p);
  }
}

// Function to return the underlying model in StommsModel.
const Model& StommsModel::getModel()
{
  return model;
}

// Function to return the vector containing all the planes with their geometric data.
const std::vector <Plane>& StommsModel::getPlanes()
{
  return planes;
}

// Function to return ModelMetaData stored in StommsModel.
const ModelMetaData& StommsModel::getModelMetaData()
{
  return modelMetaData;
}

// From the Model, sort the oPoints by planes.
std::map<int, Vertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles)
{
  // Step 1: Fetch the Simmetrix model (pGModel) from Model m and the vmecFlux data (vf) from 
  // the pGModel. Also read the plane information from plane meta data.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = planeAngles;

  // Step 2: Look at the Opoint at each plane (zeta value) and set it to the map.
  std::map <int, Vertex> planesAxisMap;
  for (int i = 0; i < zetas.size(); i++)
  {
    pGVertex axis = VmecFlux_opointVertex(vf, zetas[i]);
    Vertex axisV;
    axisV.setSimVertex(axis);
    planesAxisMap[i] = axisV;
  }
  
  // Return the map between plane number and Opoints.
  return planesAxisMap;
}

// From the Model, sort the model entities by planes. This results defining
// each plane using its model entities (model faces for now).
std::map<int,std::vector<Face>> sortFacesByPlanes(Model m, std::vector <double> planeAngles) 
{
  // Step 1: Fetch the Simmetrix model (pGModel) from Model m and the vmecFlux data (vf) from 
  // the pGModel. Also read the plane information from plane meta data.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = planeAngles;

  // Step 2: Iterate over the model faces, read their toroidal angle and compare it to data 
  // in planes vector (zetas) to sort the model faces according to their plane number.
  std::map <int, std::vector<Face>> planesFaceMap;
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
    // If similar found, push the model face (as type Face) to respective plane number in planesMap.
    for (int i = 0; i < zetas.size(); i++)
    {
      if (zetas[i] - zeta < 1e-16)
        index = i;
    }

    Face modelFace;
    modelFace.setSimFace(gf);
    planesFaceMap[index].push_back(modelFace);
  }
  GFIter_delete(fIter); 

  // Step 3: Returns the map between plane number and associated vector of model faces.
  return planesFaceMap;
}

// Set all the flux curves on a plane.
std::vector <Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md)
{
  // Step 1: Fetch the Simmetrix model (pGModel) from Model m and the vmecFlux data (vf) from 
  // the pGModel. Also read the plane information from plane meta data.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  double zeta = md[planeNum].getPlaneToroidalAngle();

  // Step 2: Get the magnetic field information associated to the plane (although current one is
  // global but it will change in future) and psi bounds.
  MagneticGeometry mg = md[planeNum].getMagneticGeometry();
  double psiAxis = mg.getPsiAxis();
  double psiLCF = mg.getPsiLCFS();

  // Step 3: Set each flux curve one by one and then push the flux curve to flux 
  // curves container.  Iterate over the fluxMeshSize Map to start with.
  std::vector <Flux> fluxCurvesOnPlane;
  std::vector <double> psiValues = md[planeNum].getPlaneFluxValues();
  std::vector <int> numVerticesOnFlux = md[planeNum].getPlaneFluxSizes();
  for( int i = 0; i < numVerticesOnFlux.size(); i++)
  {
    // Step 3.1: Don't take any action for the O-point
    if (numVerticesOnFlux[i] == 1)
      continue;  // Ignore the psi value at Opoint

    // Step 3.2: Declare a Flux and assign data to its members.
    Flux f;
    f.planeNumber = planeNum;
    f.psiNormOnFlux = psiValues[i];

    // Step 3.3: Get the actual psi value from normalized psi and then use the value
    // to retrieve model edge associated to it.Save it as type Edge. 
    double psi = convertNormToPsi(psiValues[i], psiAxis, psiLCF); 
    pGEdge ge = VmecFlux_poloidalEdge(vf, psi, zeta);
    Edge modelEdge;
    modelEdge.setSimEdge(ge);    

    // Step 3.4: Set the edges in a container and assign remaining member variables of Flux
    f.edgesOnFlux.push_back(modelEdge);  // For now, its a single edge. In future, for open edges we will need to store multiple edges in a container.
    f.meshVerticesOnFlux = numVerticesOnFlux[i];   

    // Step 3.5: Push the flux curves to a container.
    fluxCurvesOnPlane.push_back(f);
  }
  
  return fluxCurvesOnPlane;
}

