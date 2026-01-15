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
  // Step 1: Setup the flux values on the plane.
  fluxValues = f.fluxInput;

  // Step 2:Setup the vector for number of desired mesh vertices on each flux curve.
  for( const auto &itr : f.fluxMeshSize)
    fluxMeshSize.push_back(itr.second);
}

// Function to return the toroidal angle of the poloidal plane.
const double& PlaneMetaData::getPlaneToroidalAngle()
{
  return toroidalAngle;
}

// Function to return the psi values of the desired flux curves on the poloidal plane.
const std::vector<double>& PlaneMetaData::getPlaneFluxValues()
{
  return fluxValues;
}

// Function to return a vector of desired number of vertices on each flux curve on the poloidal plane.
const std::vector <int>& PlaneMetaData::getPlaneFluxSizes()
{
  return fluxMeshSize;
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
const PlaneMetaData& ModelMetaData::getPlaneMetaDataByIndex(int index)
{
  return planesContainer[index];
}

// Function to return a vector of toroidal angles of all the planes.
const std::vector <double>& ModelMetaData::getToroidalAnglesMetaData()
{
  return planesToroidalAngles;
}
