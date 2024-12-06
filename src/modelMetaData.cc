#include "modelMetaData.h"
#include "modelTopology.h"
#include <iterator>

// Constructor gets the flux data (f), magnetic geometry (mg), and plane toroidal angle to setup meta data on the plane.
PlaneMetaData::PlaneMetaData(const FluxData& f, const MagneticGeometry& magGeom, double angle):toroidalAngle(angle),mg(magGeom)
{
  // Step 1: Setup the flux values on the plane.
  fluxValues = f.fluxInput;

  // Step 2: Find any critical points on plane (Future Task).
  //oPoints = searchOPoints(magGeom);
  //xPoints = searchXPoints(magGeom);
 
  // Step 3:Setup the vector for number of desired mesh vertices on each flux curve.
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

// Function to return the magnetic geometry set on the plane meta data.
const MagneticGeometry& PlaneMetaData::getMagneticGeometry()
{
  return mg;
}

// Function to add individual plane meta data to the object of class ModelMetaData.
void ModelMetaData::addPlane(PlaneMetaData pg)
{
  // Step 1: Get the individual plane meta data and save it to the vector of planes (planesContainer).
  planesContainer.push_back(pg);

  // Step 2: Retrieve the toroidal angle of each plane and save it to the vector holding 
  // the plane angles data.
  double toroidalAngle = pg.getPlaneToroidalAngle();
  planesToroidalAngles.push_back(toroidalAngle);
}

// Function to return a vector containing all the planes with their meta data.
const std::vector <PlaneMetaData>& ModelMetaData::getPlanesContainer()
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
