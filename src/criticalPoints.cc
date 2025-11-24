#include "criticalPoints.h"

/***********************************************/
// Structs for Simplex Method
// 1- Simplex 
// 2- Simplex Grid
/***********************************************/
Simplex::Simplex(Point pt1, Point pt2, Point pt3):point1(pt1), point2(pt2), point3(pt3){}
Simplex::Simplex(std::vector <Point> pts)
{
  assert (pts.size() == 3 || pts.size() == 4); // 3 for 2D, 4 for 3D
  point1 = pts[0];
  point2 = pts[1];
  point3 = pts[2];
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
