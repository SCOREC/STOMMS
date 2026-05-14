#include "criticalPoints.h"

/***********************************************/
// Class: PhysicsPoint
// Class to define and access physics of a
// physical point.
/***********************************************/
PhysicsPoint::PhysicsPoint(const Point& point, const double& psiAtPoint, const PhysicsPointType pType)
{
  pt = point;
  psi = psiAtPoint;
  pointType = pType;
}

// Function to get physical coordinates of a point.
const Point& PhysicsPoint::getPoint() const
{
  return pt;
}

// Function to get psi value at a point.
const double& PhysicsPoint::getPsi() const
{
  return psi;
}

// Function to get point type.
const PhysicsPointType& PhysicsPoint::getPointType() const
{
  return pointType;
}

/***********************************************/
// Helper Functions 
/***********************************************/

// If the given Point pt is slightly outside the domain box due to
// floating points, readjust it to the boundary of box
void checkBounds(Point& pt, const std::array<double,4>& box)
{
  // Step 1: Get the lower and upper limits of rectangular box.
  double xMin = box[0], yMin = box[1];
  double xMax = box[2], yMax = box[3];

  // Step 2: If point is outside the box and within tolerance 
  // (1e-8) set point to the corresponding bound.
  if (pt.x < xMin && (fabs(pt.x - xMin) < 1e-8))
    pt.x = xMin;
  if (pt.x > xMax && (fabs(pt.x - xMax) < 1e-8))
    pt.x = xMax;
  if (pt.y < yMin && (fabs(pt.y - yMin) < 1e-8))
    pt.y = yMin;
  if (pt.y > yMax && (fabs(pt.y - yMax) < 1e-8))
    pt.y = yMax;
}

// Check if the given point pt is inside or outside of the domain box.
bool inDomain(const Point& pt, const std::array<double,4>& box)
{
  // Step 1: Get the lower and upper limits of rectangular box.
  double xMin = box[0], yMin = box[1];
  double xMax = box[2], yMax = box[3];

  // Step 2: If point is outside the box set, return false.
  if (pt.x < xMin || pt.x > xMax || pt.y < yMin || pt.y > yMax)
    return false;

  return true;
}

// Given a vector of points, filter the unique points and get rid of duplicates.
std::vector <Point> filterUniquePoints(const std::vector <Point>& candidates)
{
  std::vector <Point> uniquePoints;  

  // Step 1: Iterate over the points
  for(int i = 0; i < candidates.size(); i++)
  {
    bool foundSame = false;
    Point ptToCheck = candidates[i];

    // Step 2: Iterate over the filtered points.
    for (int j = 0; j < uniquePoints.size(); j++)
    {
      Point uniquePt = uniquePoints[j];

      // Step 3: If any point from candidates is sames as one of the 
      // filtered point, break the inner loop and set foundSame to true.
      if (arePointsSame(ptToCheck, uniquePt))
      {
        foundSame = true;
        break;
      }
    }
    
    // Step 4: If foundSame is true, ignore the point, else save it to
    // uniquePoints vector.
    if (foundSame)
      continue;

    uniquePoints.push_back(candidates[i]);
  }

  // Step 5: Return unique points.
  return uniquePoints;
}

// Given a vector of points, filter out the points that are outside the wall curve.
void filterOutsideTheWallPoints(std::vector <Point>& candidates, const std::vector <Point>& wall)
{
  for (int  i = 0; i < candidates.size(); i++)
  {
    Point pt = candidates[i];
  
    // Step 1: If outside the wall curve, erase it from vector and 
    // reset iterator.
    if (windingNumberPolygonTest(pt, wall) == 0)
    {
      candidates.erase(candidates.begin()+i);
      i--;
    }
  }
}

// Given a vector of critical points, print them out.
void printCriticalPoints(const std::vector <PhysicsPoint>& criticalPoints)
{
  // Step 1: Determine the type of critical point. This could be checked by checking 
  // the type of one of the points in the vector. Print out the type.
  std::string pointType;
  if (criticalPoints.size() > 0)
  {
    criticalPoints[0].getPointType() == PhysicsPointType::OPoint ? pointType = "O Point" : pointType = "X Point";
    std::cout << "\t* " << pointType << " * \n\n";
  }

  // Step 2: Iterate over the rest of the points and print them out.
  for (int i = 0; i < criticalPoints.size(); i++)
  {
    Point pt = criticalPoints[i].getPoint();
    std::cout << "\t\tPosition of " << pointType << " # " << i+1 << " (R,Z) = (" << pt.x << ", " << pt.y << ")\n";
    std::cout << "\t\tMagnetic flux (psi) " << pointType << " # " << i+1 << " = " << criticalPoints[i].getPsi() << "\n";
  }
}

// Function to compare two physics points value. Need to feed this function 
// to std::sort.
bool comparePhysicsPoints(const PhysicsPoint& pt1, const PhysicsPoint& pt2)
{
  return pt1.getPsi() <= pt2.getPsi();
}
 
