#include "criticalPoints.h"

/***********************************************/
// Class: PhysicsPoint
// Class to define and access physics of a
// physical point.
/***********************************************/
PhysicsPoint::PhysicsPoint(const Point& point, const double& psiAtPoint, const PointType pType)
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
const PointType& PhysicsPoint::getPointType() const
{
  return pointType;
}


// Helper Functions (Add comments later)
void checkBounds(Point& pt, std::array<double,4> box)
{
  double xMin = box[0], yMin = box[1];
  double xMax = box[2], yMax = box[3];

  // If point is outside the box and within tolerance (1e-8)
  // set point to the corresponding bound.
  if (pt.x < xMin && (fabs(pt.x - xMin) < 1e-8))
    pt.x = xMin;
  if (pt.x > xMax && (fabs(pt.x - xMax) < 1e-8))
    pt.x = xMax;
  if (pt.y < yMin && (fabs(pt.y - yMin) < 1e-8))
    pt.y = yMin;
  if (pt.y > yMax && (fabs(pt.y - yMax) < 1e-8))
    pt.y = yMax;
}

bool inDomain(const Point& pt, std::array<double,4> box)
{
  double xMin = box[0], yMin = box[1];
  double xMax = box[2], yMax = box[3];

  // If point is outside the box and within tolerance (1e-8)
  // set point to the corresponding bound.
  if (pt.x < xMin || pt.x > xMax || pt.y < yMin || pt.y > yMax)
    return false;

  return true;
}

std::vector <Point> filterUniquePoints(const std::vector <Point>& candidates)
{
  std::vector <Point> uniquePoints;

  for(int i = 0; i < candidates.size(); i++)
  {
    bool foundSame = false;
    Point ptToCheck = candidates[i];
    for (int j = 0; j < uniquePoints.size(); j++)
    {
      Point uniquePt = uniquePoints[j];
      if (arePointsSame(ptToCheck, uniquePt))
      {
        foundSame = true;
        break;
      }
    }
    if (foundSame)
      continue;

    uniquePoints.push_back(candidates[i]);
  }

  return uniquePoints;
}



void printCriticalPoints(const std::vector <PhysicsPoint>& criticalPoints)
{
  std::string pointType;
  criticalPoints[0].getPointType() == PointType::OPoint ? pointType = "O Point" : pointType = "X Point";

  std::cout << "\t* " << pointType << " * \n\n";
  for (int i = 0; i < criticalPoints.size(); i++)
  {
    Point pt = criticalPoints[i].getPoint();
    std::cout << "\t\tPosition of " << pointType << " # " << i+1 << " (R,Z) = (" << pt.x << ", " << pt.y << ")\n";
    std::cout << "\t\tMagnetic flux (psi) " << pointType << " #" << i+1 << " = " << criticalPoints[i].getPsi() << "\n";
  }
} 
