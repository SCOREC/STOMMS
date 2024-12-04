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



