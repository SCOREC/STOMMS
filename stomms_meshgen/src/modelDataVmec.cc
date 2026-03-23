#include "modelDataVmec.h"

/***********************************************/
// Class ModelDataVmec
// creates a geometry based on model meta data,
// and sets properties of model based on 
// specific magnetic input type.
/***********************************************/
// Constructor gets the model meta data and uses it to setup StommsModel.
ModelVmec::ModelVmec(const ModelMetaData& md, const VmecData& vm): modelMetaData(md), vmec(vm)
{
  // Step 1: Get the vector of planes with their meta data from ModelMetadata.
  planesContainer = modelMetaData.getPlanesContainer();

  // Step 2: Generate the model using planes meta data.
  model = generateCoreSimModelVmec(planesContainer, vmec);

  // Step 3: Setup the poloidal planes with their model enteties using
  // the model information.
  setPlanes();

  // Step 4: Classify model faces.
  classifyModelFaces();

  // Step 5: Set parametric values of mesh vertices on the flux curves.
  setMeshVerticesOnPlanes();
}

// Setting model entities from Simmetrix Model (pGModel) to respective planes.
void ModelVmec::setPlanes()
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

// From the Model, sort the oPoints by planes.
std::map<int, Vertex> ModelVmec::sortOPointsByPlanes(Model m, std::vector <double> planeAngles)
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
    
    // Step 2.1: Set the attributes to the vertex.
    GEN_setNativeIntAttribute(axis, static_cast<int>(PointType::OPoint), "PointType");
    GEN_setNativeDoubleAttribute(axis, 0.0, "PsiNorm");    

    axisV.setSimVertex(axis);
    planesAxisMap[i] = axisV;
  }
  
  // Return the map between plane number and Opoints.
  return planesAxisMap;
}

// From the Model, sort the model entities by planes. This results defining
// each plane using its model entities (model faces for now).
std::map<int,std::vector<Face>> ModelVmec::sortFacesByPlanes(Model m, std::vector <double> planeAngles) 
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

    // Step 2.3: Set up model face of type Face and push it back in the vector of faces.
    Face modelFace;
    modelFace.setSimFace(gf);
    planesFaceMap[index].push_back(modelFace);
  }
  GFIter_delete(fIter); 

  // Step 3: Returns the map between plane number and associated vector of model faces.
  return planesFaceMap;
}

// Set all the flux curves on a plane.
std::vector <Flux> ModelVmec::setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md)
{
  // Step 1: Fetch the Simmetrix model (pGModel) from Model m and the vmecFlux data (vf) from 
  // the pGModel. Also read the plane information from plane meta data.
  pGModel model = m.getSimModel();
  pVmecFlux vf = GM_vmec(model);
  double zeta = md[planeNum].getPlaneToroidalAngle();

  // Step 2: Get the magnetic field information associated to the plane (although current one is
  // global but it will change in future) and psi bounds.
  double psiAxis = vmec.psi[0];
  double psiLCF = vmec.psi[vmec.nSurf - 1];

  // Step 3: Set each flux curve one by one and then push the flux curve to flux 
  // curves container.  Iterate over the fluxMeshSize Map to start with.
  std::vector <Flux> fluxCurvesOnPlane;
  std::vector <double> psiValues = md[planeNum].getPlaneFluxValues();
  std::vector <double> nodeSpacingOnFlux = md[planeNum].getPlaneFluxSizes();
  for( int i = 0; i < nodeSpacingOnFlux.size(); i++)
  {
    // Step 3.1: Don't take any action for the O-point
    if (i == 0)
      continue;  // Ignore the psi value at Opoint

    // Step 3.2: Declare a Flux and assign data to its members.
    Flux f;
    f.planeNumber = planeNum;
    f.psiNormOnFlux = psiValues[i];
    f.curveType = CurveType::Closed;

    // Step 3.3: Get the actual psi value from normalized psi and then use the value
    // to retrieve model edge associated to it.Save it as type Edge. 
    double psi = convertNormToPsi(psiValues[i], psiAxis, psiLCF); 
    pGEdge ge = VmecFlux_poloidalEdge(vf, psi, zeta);
    Edge modelEdge;
    modelEdge.setSimEdge(ge);    
  
    // Step 3.4: Set edge properties in terms of attributes on pGEdge.
    GEN_setNativeIntAttribute(ge, static_cast<int>(CurveType::Closed), "CurveType");
    GEN_setNativeDoubleAttribute(ge, psiValues[i], "PsiNorm");

    // Step 3.5: Set the edges in a container and assign remaining member variables of Flux
    f.edgesOnFlux.push_back(modelEdge);  // For now, its a single edge. In future, for open edges we will need to store multiple edges in a container.
    f.nodeSpacingOnFlux = nodeSpacingOnFlux[i];

    // Step 3.6: Push the flux curves to a container.
    fluxCurvesOnPlane.push_back(f);
  }
  
  return fluxCurvesOnPlane;
}

// Classify Model Faces
void ModelVmec::classifyModelFaces()
{
  // Step 1: Get all the faces on the model.
  std::vector <Face> modelFaces = model.getModelFaces();

  // Step 2: Iterate over the face and check if they belong to core, sol, pvt, or any other physics region.
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];
    pGFace gf = f.getSimFace();

    // Step 2.1: If belongs to core, set the face attribute.
    if (isModelFaceOnCore(gf))
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::Core), "PhysicsRegion");
  }
}

// For a set of planes, set the mesh vertices on the flux curves on all plane.
void ModelVmec::setMeshVerticesOnPlanes()
{
  // Step 1: Set the values on flux curves of plane 0.
  setMeshVerticesOnPlane(0);
  
  // Step 2: Get the points on plane 0 and set them on remaining planes.
  std::vector <FluxParametricPoints> points = planes[0].getFieldPointsOnFluxCurves();
  for (int i = 1; i < planes.size(); i++)
  {
    std::vector <FluxParametricPoints> fluxPointsOnPlane;
    std::vector <Flux> fluxCurves = planes[i].fluxCurves;

    // Step 2.1: Iterate over flux curves of the plane 
    for (int i = 0; i < fluxCurves.size(); i++)
    {
      Flux f = fluxCurves[i];
      std::vector <std::vector<double>> parValues = points[i].getParametricValuesOnFlux();
      FluxParametricPoints parOnFlux(f, parValues);
      fluxPointsOnPlane.push_back(parOnFlux);
    }

    // Step 2.2: Set values back in the plane.
    planes[i].setFieldPointsOnFlux(fluxPointsOnPlane);      
  }
}

// Given a single plane, set mesh vertices on the flux curves that belong to the plane.
void ModelVmec::setMeshVerticesOnPlane(int planeIndex)
{
  std::vector <FluxParametricPoints> fluxPointsOnPlane;

  // Step 1: Iterate over the flux curves on the plane and set field points on them.
  std::vector <Flux> fluxCurves = planes[planeIndex].fluxCurves;
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    Flux f = fluxCurves[i];

    // Step 1.1: Get the par values of points on the flux curve for field following.
    int pointsPlacementType = 0;
    FluxParametricPoints points(f, pointsPlacementType);
    fluxPointsOnPlane.push_back(points);
  }
  planes[planeIndex].setFieldPointsOnFlux(fluxPointsOnPlane); 
}

// Function to get model associated with vmec geometry.
const Model& ModelVmec::getModel() const
{
  return model;
}

// Function to get all the geometric information on individual planes.
const std::vector <Plane>& ModelVmec::getPlanes() const
{
  return planes;
}
