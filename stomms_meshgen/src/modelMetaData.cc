#include "modelMetaData.h"
#include "modelTopology.h"
#include <iterator>

/***********************************************/
// Class PlaneMetaData
// Sets metadata on a poloidal plane.
/***********************************************/

// Constructor gets the flux data (f), magnetic geometry (mg), and plane toroidal angle to setup meta data on the plane.
PlaneMetaData::PlaneMetaData(const FluxData& f, double angle):toroidalAngle(angle)
{
  // Step 1: Store the mesh sizes on each flux directly from the mesh size file
  // as they are given. 
  for( const auto &itr : f.fluxMeshSize)
  {
    rawFluxValuesFromFile.push_back(itr.first);  // flux values
    rawMeshSizesFromFile.push_back(itr.second);  // corresponding mesh sizes
  }
  
  // Step 2: Update the fluxValues and mesh size vectors to make sure we get a 
  // consistent data between f.fluxInput and mesh size vectors. This requires
  // interpolation.
  updateMeshSizeVector(f.fluxInput);

  // Step 3: Adding a guard to make sure vectors are updated correctly.
  assert (fluxValues.size() == fluxMeshSize.size());
}

// Function to set mesh size on the model faces with unstructured mesh.
void PlaneMetaData::setSizeForUnstructuredMesh(double meshSize)
{
  meshSizeUnstructured = meshSize;
}

// Function to return the toroidal angle of the poloidal plane.
const double& PlaneMetaData::getPlaneToroidalAngle() const
{
  return toroidalAngle;
}

// Function to return the psi values of the desired flux curves on the poloidal plane.
const std::vector<double>& PlaneMetaData::getPlaneFluxValues()const
{
  return fluxValues;
}

// Function to return a vector of desired node spacing on each flux curve on the poloidal plane.
const std::vector <double>& PlaneMetaData::getPlaneFluxSizes() const
{
  return fluxMeshSize;
}

// Function to return mesh size at specific psi normalized value.
double PlaneMetaData::getNodeSpacingAtFlux(double psiNorm)
{
  // Step 1: Check bounds and if values are lower and upper bounds use value of lowest or highest
  // index accordingly. First take care of outside the range values.
  if (psiNorm <= rawFluxValuesFromFile.front())
    return rawMeshSizesFromFile.front();
  if (psiNorm >= rawFluxValuesFromFile.back())
    return rawMeshSizesFromFile.back();

  // Step 2: If not out of bounds, find the index of first value in the 
  // rawFluxValuesFromFile vector that is equal or greater than given psiNorm. 
  int indx = 1;
  while (rawFluxValuesFromFile[indx] < psiNorm)
    indx++;	

  // Step 3: Linear Interpolation (y = (y2 - y1)/(x2 - x1)*(x - x1) + y1)
  double y1 = rawMeshSizesFromFile[indx - 1];
  double y2 = rawMeshSizesFromFile[indx];
  double x1 = rawFluxValuesFromFile[indx - 1];
  double x2 = rawFluxValuesFromFile[indx];
  double meshSize = (y2 - y1)/(x2 - x1)*(psiNorm - x1) + y1;
  return meshSize;
}

// Function to update the internal vectors of psi values and mesh sizes to make
// sure we have a value of mesh size for every psi value. Done using interpolation.
void PlaneMetaData::updateMeshSizeVector(const std::vector<double>& psiValues)
{
  // Step 1: Interpolate the mesh size at each flux psi value.
  std::vector <double> meshSizes;
  for (int i = 0; i < psiValues.size(); i++)
  {
    double psiNorm = psiValues[i]; 
    meshSizes.push_back(getNodeSpacingAtFlux(psiNorm));
  }

  // Step 2: Update the vectors of flux values and its corresponding mesh sizes.
  fluxValues = psiValues;
  fluxMeshSize = meshSizes;
}

// Function to get a vector of mesh node spacing (map directly with
// the flux curve vector).
const std::vector <double>& PlaneMetaData::getNodeSpacingVector() const
{
  return fluxMeshSize;
}

// Get mesh size on the model faces with unstructured mesh.
const double& PlaneMetaData::getSizeForUnstructuredMesh() const
{
  return meshSizeUnstructured;
}

/***********************************************/
// Class ModelMetaData
// sets metadata for the model (all the planes)
/***********************************************/

// ModelMetaData Constructor
ModelMetaData::ModelMetaData(const Inputs& inputs)
{
  // Step 1: Set up planer metadata
  for (int i = 0; i < inputs.getInputData().pd.planeInput.size(); i++)
  {
    double toroidalAngle = inputs.getInputData().pd.planeInput[i];
    PlaneMetaData pg(inputs.getInputData().fd, toroidalAngle);
    pg.setSizeForUnstructuredMesh(inputs.getMeshSizeUnstructured());
    planesContainer.push_back(pg);
    planesToroidalAngles.push_back(toroidalAngle);
  }
}

// Function to return a vector containing all the planes with their meta data.
const std::vector <PlaneMetaData>& ModelMetaData::getPlanesContainer() const
{
  return planesContainer;
}

// Function to return the metadata on a individual plane by index.
const PlaneMetaData& ModelMetaData::getPlaneMetaDataByIndex(int index) const
{
  return planesContainer[index];
}

// Function to return a vector of toroidal angles of all the planes.
const std::vector <double>& ModelMetaData::getToroidalAnglesMetaData()
{
  return planesToroidalAngles;
}
