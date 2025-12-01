#include "criticalPoints.h"

/***********************************************/
// Structs for Simplex Method
// 1- Simplex 
// 2- Simplex Grid
/***********************************************/
// Simplex
Simplex::Simplex(Point pt1, Point pt2, Point pt3):point1(pt1), point2(pt2), point3(pt3){}
Simplex::Simplex(std::vector <Point> pts)
{
  assert (pts.size() == 3 || pts.size() == 4); // 3 for 2D, 4 for 3D
  point1 = pts[0];
  point2 = pts[1];
  point3 = pts[2];
}

// Simplex Grid
SimplexGrid::SimplexGrid(int xResolution, int yResolution, std::array<double,4> box)
{
  double xMin = box[0], yMin = box[1];
  double xMax = box[2], yMax = box[3];
  for (int i = 0; i < xResolution; i++)
  {
    for (int j = 0; j < yResolution; j++)
    {
      Point p1, p2, p3;
      p1.z = p2.z = p3.z = 0.0;

      // Point 1
      p1.x = (xMin*(xResolution-i) + xMax*i)/double(xResolution);
      p1.y = (yMin*(yResolution-j) + yMax*j)/double(yResolution);

      // Point 2
      p2.x = (xMin*(xResolution-i-1) + xMax*(i+1))/double(xResolution);
      p2.y = p1.y;

      // Point 3
      p3.x = p1.x;
      p3.y = (yMin*(yResolution-j-1) + yMax*(j+1))/double(yResolution);

      // To make sure we are not getting a point just outside of the
      // computational domain because of numerical inconsistencies.
      checkBounds(p1, box);
      checkBounds(p2, box);
      checkBounds(p3, box);

      // Create Simplex
      Simplex simplex(p1,p2,p3);
      grid.push_back(simplex);
    }
  }
  assert (grid.size() == (xResolution*yResolution));
}

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


//CLASS: 
CriticalPointsOnPlane::CriticalPointsOnPlane()
{
  std::vector <PhysicsPoint> candidates;
    
}

double getHessianAtPoint(const Point& pt)
{
  int ier;
  double d2[3];

  bool reversePsi = false;  // for now, change it to argument later or define field some other way
  // Step 1: Evaluate d^2(psi)/dr^2
  int dr = 2, dz = 0;
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &d2[0], &ier, reversePsi);
  assert(ier == 0);

  // Step 2: Evaluate dpsi/drdz
  dr = dz = 1;
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &d2[1], &ier, reversePsi);
  assert(ier == 0);

  // Step 3: Evaluate d^2(psi)/dz^2
  dr = 0, dz = 2;
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &d2[2], &ier, reversePsi);
  assert(ier == 0);

  // Step 4: Second Partial Derivative Test
  // https://en.wikipedia.org/wiki/Second_partial_derivative_test
  double det = d2[0]*d2[2] - d2[1]*d2[1];
  assert(det != 0);

  return det;
}

std::vector <PhysicsPoint> CriticalPointsOnPlane::findMinimumSimplexMethod()
{
  // Step 1: Domain box bounds
  double bbox[4]; // min r, min z, max r, max z
  get_b_box_(bbox);
  domainBox = {bbox[0], bbox[1], bbox[2], bbox[3]};

  // Step 2: Set up variables for Simplex method and field values
  std::array<std::array<double, 2>, 3> simplexPoints;
  double relativeTolerance = 1E-8;
  double functionTolerance = 1E-8;
  int nfunc;

  // Now variables for field values (psi values and its gradients)
  std::array<double, 3> gradPsi;
  double psi;
  int ier;

  // Step 3: Set up 
 
  std::vector <PhysicsPoint> minimumPoints;
  return minimumPoints; 
}

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
