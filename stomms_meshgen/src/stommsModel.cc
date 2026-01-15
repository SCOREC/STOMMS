#include "stommsModel.h"

/***********************************************/
// Class: StommsModel
/***********************************************/

// Constructor gets the model meta data and uses it to setup StommsModel.
StommsModel::StommsModel(std::shared_ptr<MagneticGeometry> mg):magneticGeometry(mg)
{
  // Step 1: Read the model file from magnetic geometry
  model = magneticGeometry->getModel();

  // Step 2: Read the planes from magnetic geometry
  planes = magneticGeometry->getPlanes();

  // Step 3: Set attributes on model entities.
  setModelAttributes();
}

// Function to setup attributes on model entities.
void StommsModel::setModelAttributes()
{
  // Set attributes on model faces.
  setModelFaceAttributes();

  // Extend it if needed for other model entity types.
}

// Function to setup attributes on model faces.
void StommsModel::setModelFaceAttributes()
{
  // Step 1: Get all the faces on the model.
  std::vector <Face> modelFaces = model.getModelFaces();

  // Step 2: Iterate over the face and check if they belong to core, sol, pvt, or any other physics region.
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];

    // Step 2.1: If belongs to core, set the face type = 1.
    int fAttribute = 0;
    if (isFaceOnCore(f))
      fAttribute = 1;

    // Step 2.2: Set the face types in attribute vector.
    faceAttributes.push_back(fAttribute);
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

// Function to check if a model face is on core or not.
bool isFaceOnCore(const Face& f)
{
  // Step 1: Get the vector of model edges on the model face.
  Face gf = f;
  std::vector <Edge> edges = gf.getEdgesOnFace();

  // Step 2: Iterate over the model edges and check how man of them are periodic. 
  int numPeriodicEdges = 0;
  for (int i = 0; i < edges.size(); i++)
  {
    Edge ge = edges[i];
    if (ge.edgeIsPeriodic())
     numPeriodicEdges++; 
  }

  // Step 3: If number of periodic edges are 2, its on core region.
  // or if its on faces adjacent to oPoint with one edge, its on core region.
  bool isCore = false;
  if (numPeriodicEdges == 2 || edges.size() == 1 && numPeriodicEdges == 1)
    isCore = true;
  
  return isCore;
}

// Function to get the physics region type of a model face.
int getModelFacePhysicsType(const Face& face)
{
  // Step 1: Get the face that needs to be checked.
  Face f = face;

  // Step 2: If face is not classified on known model types, set it to
  // 0, if on core set it to 1, and extend accordingly in future.
  int physicsType = 0;
  if (isFaceOnCore(f))
    physicsType = 1;

  // Step 3: Return the physics type.
  return physicsType;
}
