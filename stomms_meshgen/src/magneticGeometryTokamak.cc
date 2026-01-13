#include "magneticGeometryTokamak.h"

/***********************************************/
// Class: MagneticGeometryForTokamak
// Derived class for MagneticGeometry
/***********************************************/
MagneticGeometryForTokamak::MagneticGeometryForTokamak(const WallCurve& wall, const bool& useReversePsi)
{
  // Step 1: Find critical points from EQDSK file and print them.
  wallCurve = wall;
  reversePsi = useReversePsi;
  CriticalPointsEqdsk criticalPoints(wall, reversePsi);
  std::cout << "Reverse Psi " << (reversePsi == true ? "ON" : "OFF" ) << "\n";

  // Step 2: Read and sort critical points.
  std::vector <PhysicsPoint> oPointsVec = criticalPoints.getOPoints();
  std::vector <PhysicsPoint> xPointsVec = criticalPoints.getXPoints();
  std::sort(oPointsVec.begin(), oPointsVec.end(), comparePhysicsPoints);
  std::sort(xPointsVec.begin(), xPointsVec.end(), comparePhysicsPoints);

  // Step 3: Print critical points
  printCriticalPoints(oPointsVec);
  printCriticalPoints(xPointsVec);

  // Step 2: Setup critical points on each plane.
  // Just one plane for tokamak
  oPoints[0] = oPointsVec;
  xPoints[0] = xPointsVec;
}

// Function to get a map between plane number and vector of OPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryForTokamak::getOPoints() const
{
  return oPoints;
}

// Function to get a map between plane number and vector of XPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryForTokamak::getXPoints() const
{
  return xPoints;
}

/* 
 * A function to return psi value of the axis in the tokamak domain.
 */
double MagneticGeometryForTokamak::getPsiAxis() const
{
  PhysicsPoint axis = oPoints.at(0).at(0);  // first member on first plane. 
  return axis.getPsi();
}

/* 
 * A function to return psi value of the innermost separatrix.
 */
double MagneticGeometryForTokamak::getPsiCoreBoundary() const
{
  PhysicsPoint xPt = xPoints.at(0).at(0);
  return xPt.getPsi();
}

// Function to return the VMEC data. Not applicable for Tokamaks. 
// Return an error message.
const VmecData& MagneticGeometryForTokamak::getVmecData() const
{
  std::cerr << "Input magnetic field is EQDSK for Tokamaks. Make sure to provide VMEC input file to use this function\n";
  std::cerr << "If not, use the correct functions to access magnetic field data from EQDSK file\n";
  exit(1);
}
