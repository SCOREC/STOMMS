#include "modelMetaData.h"
#include "modelTopology.h"
#include <iterator>

PlaneMetaData::PlaneMetaData(const FluxData& f, const MagneticGeometry& magGeom, double angle):toroidalAngle(angle),mg(magGeom)
{
  // Step 1: Setup the flux values on the plane.
  fluxValues = f.fluxInput;

  // Step 2: Find any critical points on plane (Future Task).
  //oPoints = searchOPoints(magGeom);
  //xPoints = searchXPoints(magGeom);
 
  // Step 3:Setup the map for number of desired mesh vertices on each flux curve.
  for( const auto &itr : f.fluxMeshSize)
    fluxMeshSize.push_back(itr.second);
}

const double& PlaneMetaData::getPlaneToroidalAngle()
{
  return toroidalAngle;
}

const std::vector<double>& PlaneMetaData::getPlaneFluxValues()
{
  return fluxValues;
}

const std::vector <int>& PlaneMetaData::getPlaneFluxSizes()
{
  return fluxMeshSize;
}

const MagneticGeometry& PlaneMetaData::getMagneticGeometry()
{
  return mg;
}



