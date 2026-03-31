#include "meshMetaData.h"

/***********************************************/
// Class: PlaneMeshMetaData
/***********************************************/

// Function to set the model plane in plane mesh meta data.
void PlaneMeshMetaData::setModelPlane(const Plane& p)
{
  // Step 1: Get the model faces on the plane.
  modelPlane = p;
  std::vector <Face> modelFaces = modelPlane.modelFaces;
 
  // Step 2: Set the mesh types on model faces.
  faceMeshType = setFaceMeshType(modelFaces);

  // Step 3: Set up the mesh vertices on each plane.
  meshVerticesParametricLocation = modelPlane.getFieldPointsOnFluxCurves();

  // Step 4: Set mesh size on the model plane.
  meshSizeUnstructured = modelPlane.getUnstructuredMeshSizeOnPlane();
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
   
    // Step 4: Setup on the private regions (TO-DO)
    int fluxCurvesOnPrivateRegion = 1;  // Get this from user
    if (fluxCurvesOnPrivateRegion && faceType == FaceType::Private)
      meshType = 1;

    // Step 5: If SOL, and adjacent to x-points, set unstructured mesh type
    std::vector <pGVertex> xPts = getCriticalPointsOnModelFace(f.getSimFace(), PointType::XPoint); 
    if ((faceType == FaceType::ScrapeOffLayer || faceType == FaceType::Core || 
         fluxCurvesOnPrivateRegion) && (xPts.size() > 0))
      meshType = 0;

    // Step 6: Store the type in the vector.
    meshTypeOnFace.push_back(meshType);
  }
  return meshTypeOnFace;
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

// Function to get a vector of set of the class FluxParametricPoints. This vector is equal 
// to the number of the flux curves. Each member contains the details of the field following 
// points parametric values.
const std::vector <FluxParametricPoints>& PlaneMeshMetaData::getMeshVerticesOnFlux() const
{
  return meshVerticesParametricLocation;
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

// Function to get unstructured mesh size on the plane.
const double& PlaneMeshMetaData::getUnstructuredMeshSizeOnPlane() const
{
  return meshSizeUnstructured;
}

/***********************************************/
// Class:: MeshMetaData
/***********************************************/
MeshMetaData::MeshMetaData(const StommsModel& m):stommsModel(m)
{
  // Step 1: Get all the model planes from model.
  std::vector <Plane> modelPlanes = stommsModel.getPlanes();

  // Step 2: Iterate over the planes (except plane 0) and set mesh meta 
  // data on each plane and save each plane to planeMeshData.
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
