#include "stommsModel.h"

/***********************************************/
// Class: StommsModel
/***********************************************/

// Constructor gets the model meta data and uses it to setup StommsModel.
StommsModel::StommsModel(std::shared_ptr<MagneticGeometry> mg):magneticGeometry(mg)
{
  std::cout << ".......... Setting STOMMS Model\n";
  // Step 1: Read the model file from magnetic geometry
  model = magneticGeometry->getModel();

  // Step 2: Read the planes from magnetic geometry
  planes = magneticGeometry->getPlanes();
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

// Function to get the physics region type of a model face.
FaceType getModelFacePhysicsType(const Face& face)
{
  // Step 1: Get the face that needs to be checked.
  Face f = face;
  pGFace gf = face.getSimFace();

  // Step 2: Find the face type set on the model face.
  assert(GEN_numNativeIntAttribute(gf, "PhysicsRegion"));
  int faceType = -1;
  GEN_nativeIntAttribute(gf, "PhysicsRegion", &faceType);
  FaceType physicsType = static_cast<FaceType>(faceType); 
  
  // Step 3: Return the physics type.
  return physicsType;
}
