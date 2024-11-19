#include "stomms.h"
#include "modelTopology.h"
#include <iterator>

PlaneGeometry::PlaneGeometry(const FluxData& f, const MagneticGeometry& magGeom, double angle):toroidalAngle(angle)
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

const double& PlaneGeometry::getPlaneToroidalAngle()
{
  return toroidalAngle;
}

const std::vector<double>& PlaneGeometry::getPlanefluxValues()
{
  return fluxValues;
}

const std::vector <int>& PlaneGeometry::getPlaneFluxSizes()
{
  return fluxMeshSize;
}

STOMMS::STOMMS(const MagneticGeometry& magGeom):mg(magGeom)
{
  // Initialize Simmetrix handlers.
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  Sim_readLicenseFile(0);

  prog = Progress_new();
  Progress_setDefaultCallback(prog);

}

STOMMS::~STOMMS()
{
  // Delete Simmetrix handlers.
  Progress_delete(prog);
  Sim_unregisterAllKeys();
  MS_exit();
}

void STOMMS::addPlane(PlaneGeometry pg)
{
  planes.push_back(pg);
}

const std::vector <PlaneGeometry>& STOMMS::getPlanesContainer()
{
  return planes;
}
