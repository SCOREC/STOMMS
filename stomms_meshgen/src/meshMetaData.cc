#include "meshMetaData.h"

// Function to set the model plane in plane mesh meta data.
void PlaneMeshMetaData::setModelPlane(const Plane& p)
{
  // Step 1: Get the model faces on the plane.
  modelPlane = p;
  std::vector <Face> modelFaces = modelPlane.modelFaces;
 
  // Step 2: Set the mesh types on model faces.
  faceMeshType = setFaceMeshType(modelFaces);

  // Step 3: Get the flux curves on the plane and iterate over them.
  // Also, set up the mesh vertices for each fux curve.
  std::vector <Flux> fluxCurves = modelPlane.fluxCurves;
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    Flux f = fluxCurves[i];

    // Step 3.1: Get the par values of points on the flux curve for field following.
    std::vector <double> parValuesOnFlux = setMeshVerticesOnFlux(f); 
    
    // Step 3.1: Push back the vertices vector to vector containing all the flux curves.
    meshVerticesLocation.push_back(parValuesOnFlux);      
  }
}

void PlaneMeshMetaData::setModelPlane(const Plane& p, std::vector <std::vector<double>> fluxParValues)
{
  // Step 1: Get the model faces on the plane.
  modelPlane = p;
  std::vector <Face> modelFaces = modelPlane.modelFaces;
 
  // Step 2: Set the mesh types on model faces.
  faceMeshType = setFaceMeshType(modelFaces);

  // Step 3: To keep consistent parametric values across the planes for field-following,
  // use the parametric values from plane 0.
  meshVerticesLocation = fluxParValues;
}

// Function to set up the mesh types for individual faces.
std::vector <int> PlaneMeshMetaData::setFaceMeshType(const std::vector <Face> geomFaces)
{
  std::vector <int> meshTypeOnFace;

  // Step 1: Iterate over the model faces in the model.
  for (int i = 0; i < geomFaces.size(); i++)
  {
    Face f = geomFaces[i];

    // Step 2: Get the physics type of model face.
    FaceType faceType = getModelFacePhysicsType(f);

    // Step 3: Based on physics type of model face, setup the mesh type.
    int meshType = 0;
    if (faceType == FaceType::Core || faceType == FaceType::ScrapeOffLayer || 
        faceType == FaceType::LowFieldSideEdge || faceType == FaceType::HighFieldSideEdge) 
      meshType = 1;  // one-element deep

    // Step 4: Store the type in the vector.
    meshTypeOnFace.push_back(meshType);
  }
  return meshTypeOnFace;
}

// Function to set up the field following points on the flux curve.
std::vector <double> PlaneMeshMetaData::setMeshVerticesOnFlux(const Flux& f)
{
  std::vector <double> parValuesOnFlux;

  // Step 1: Get the model edge on flux curve and parameric bounds of the edge.
  Edge ge = f.edgesOnFlux[0];
  std::vector <double> parR = ge.getEdgeParRange();

  // Step 2: Push the first parametric value.
  parValuesOnFlux.push_back(parR[0]);

  // Step 3: Get the parametric values of the points satisfying nodeSpacing on flux.
  double nodeSpacing = f.nodeSpacingOnFlux;
  double parCurrent = parR[0];
  double parEnd = parR[1];
  while (ge.getEdgePartialLength(parCurrent, parEnd) > nodeSpacing)
  {
    double targetPar = getNextParamPointForDist(ge, parCurrent, parEnd, nodeSpacing);
    parValuesOnFlux.push_back(targetPar);
    parCurrent = targetPar;
  }  

  return parValuesOnFlux;
}

double PlaneMeshMetaData::getNextParamPointForDist(const Edge& ge, double parStart, double parEnd, double targetLength)
{
  // Step 1: Setup variables needed in the call.
  double lengthTolerance = 1e-5;
  int maxIter = 100;
  int numIter = 0;
  double low = parStart;
  double high = parEnd;
  double parTest;  

  // Step 2: Use bisectioning method to narrow down the exact param value for the targetLength.
  while (numIter < maxIter)
  {
    // Step 2.1: Use bisectioning method (get the half way parameteric value).
    parTest = 0.5*(low + high);
    
    // Step 2.2: Get the length from start of curve to mid point.
    double length = ge.getEdgePartialLength(parStart, parTest);

    // Step 2.3: If close to targetLength, exit the loop, else adjust the parTest value.
    if (fabs(length - targetLength) < lengthTolerance)
      break;
    else if (length < targetLength)
      low = parTest;
    else 
      high = parTest;

    numIter++;
  }
  
  return parTest;
}

// Function to return the vector of model faces on the particular poloidal plane.
const std::vector <Face>& PlaneMeshMetaData::getModelFacesOnPlane()
{
  return modelPlane.modelFaces;
}

// Function to get the vector holding the mesh type for each individual model face on the plane.
const std::vector <int>& PlaneMeshMetaData::getFaceMeshType()
{
  return faceMeshType;
}

// Function to get a vector of flux curves on the poloidal plane.
const std::vector <Flux>& PlaneMeshMetaData::getFluxCurvesOnPlane()
{
  return modelPlane.fluxCurves;
}

// Function to get a vector of set of points for the field following 
const std::vector <std::vector<double>>& PlaneMeshMetaData::getMeshVerticesOnFlux()
{
  return meshVerticesLocation;
}

// Function to get model vertex on oPoint of  the poloidal plane.
const Vertex& PlaneMeshMetaData::getOPointOnPlane()
{
  return modelPlane.oPoint;
}

// Function to get poloidal plane number.
const int& PlaneMeshMetaData::getPlaneNumber()
{
  return modelPlane.planeNumber;
}

// MeshMetaData class functions.
MeshMetaData::MeshMetaData(const StommsModel& m):stommsModel(m)
{
  // Step 1: Get all the model planes from model.
  std::vector <Plane> modelPlanes = stommsModel.getPlanes();

  // Step 2: Setup the first plane differently. Need parametric values for mesh 
  // vertices and these values will be copied to other planes.
  PlaneMeshMetaData pZeroMeshData;
  pZeroMeshData.setModelPlane(modelPlanes[0]);
  planeMeshData.push_back(pZeroMeshData);  

  // Step 3: Iterate over the planes (except plane 0) and set mesh meta 
  // data on each plane and save each plane to planeMeshData.
  for (int i = 1; i < modelPlanes.size(); i++)
  {
    PlaneMeshMetaData pMeshData;
    Plane p = modelPlanes[i];
    pMeshData.setModelPlane(p, pZeroMeshData.getMeshVerticesOnFlux());
    planeMeshData.push_back(pMeshData);
  }
}

// Function to return StommsModel from mesh meta data.
const StommsModel& MeshMetaData::getStommsModel()
{
  return stommsModel;
}

// Function to get the vector of planes mesh meta data.
const std::vector <PlaneMeshMetaData>& MeshMetaData::getMeshMetaDataPlanes()
{
  return planeMeshData;
}
