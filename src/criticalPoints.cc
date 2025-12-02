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

