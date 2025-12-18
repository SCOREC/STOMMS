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

// Function to set up the mesh types for individual faces.
std::vector <int> PlaneMeshMetaData::setFaceMeshType(const std::vector <Face> geomFaces)
{
  std::vector <int> meshTypeOnFace;

  // Step 1: Iterate over the model faces in the model.
  for (int i = 0; i < geomFaces.size(); i++)
  {
    Face f = geomFaces[i];

    // Step 2: Get the physics type of model face.
    int faceType = getModelFacePhysicsType(f);

    // Step 3: Based on physics type of model face, setup the mesh type.
    int meshType = 0;
    if (faceType == 1) // Core Region
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

  // Step 2: Based on desired number of mesh vertices, define the parametric interval between points.
  int numVert = f.meshVerticesOnFlux;
  double parInterval = (parR[1] - parR[0])/numVert;

  // Step 3: Start with parR[0] as starting point, and then keep adding interval in the loop to keep
  // the desired parametric value updated.
  double currentPar = 0.0;
  currentPar = parR[0];
  parValuesOnFlux.push_back(currentPar);
  int numIter = 1;  
  while (numIter < numVert)
  {
    currentPar += parInterval;
    parValuesOnFlux.push_back(currentPar);
    numIter++;
  }

  return parValuesOnFlux;
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

  // Step 2: Iterate over the planes and set mesh meta data on each plane.
  // and save each plabe to planeMeshData.
  for (int i = 0; i < modelPlanes.size(); i++)
  {
    PlaneMeshMetaData pMeshData;
    Plane p = modelPlanes[i];
    pMeshData.setModelPlane(p);
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
