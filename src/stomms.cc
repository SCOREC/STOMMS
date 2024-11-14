#include "stomms.h"

PlaneGeometry::PlaneGeometry(const FluxData& f, const MagneticGeometry& magGeom, double angle):toroidalAngle(angle)
{
  // Step 1: Setup the flux values on the plane.
  fluxValues = f.fluxInput;

  // Step 2: Find any critical points on plane (Future Task).
  //oPoints = searchOPoints(magGeom);
  //xPoints = searchXPoints(magGeom);
  
  // Step 3:Setup the map for number of desired mesh vertices on each flux curve.
  fluxMeshSize = f.fluxMeshSize;
}

double PlaneGeometry::getPlaneToroidalAngle()
{
  return toroidalAngle;
}

std::vector<double> PlaneGeometry::getPlanefluxValues()
{
  return fluxValues;
}

std::map <double, int> PlaneGeometry::getPlaneFluxSizes()
{
  return fluxMeshSize;
}

STOMMS::STOMMS(const MagneticGeometry& magGeom):mg(magGeom){}

void STOMMS::addPlane(PlaneGeometry pg)
{
  planes.push_back(pg);
}

std::vector <PlaneGeometry> STOMMS::getPlanesContainer()
{
  return planes;
}
