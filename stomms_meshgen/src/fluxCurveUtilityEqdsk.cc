#include "fluxCurveUtilityEqdsk.h"

bool findNextFieldFollowingPoint(Point& startPoint, Point& point1, double& dist, int m, CurveMetaData& curveData, 
                                 EqdskData eqdsk, const DomainBox& box)
{
  int steps;
  double stepToroidalAngle = getStepToroidalAngle(eqdsk.getNumPlanes(), m, steps, eqdsk.getStepRadians());

  double goal = dist;
  dist = 0.0;
  double distNewToStartLast = 0.0;

  point1 = startPoint;
  Point curveStart = curveData.origin;
  Point point2; // point to hold updated values

  // Move the step
  int numIterations = 0;
  int maxIterationsAllowed = 100000; 
  double stepToroidalMeter = 0.0;
  while (true)
  {
    stepToroidalMeter = point1.x*stepToroidalAngle;
    int res = eqdsk.rk4(point1, point2, stepToroidalMeter, 3);
    point2.z = 0.0;

    // Step : If point is outside the domain, try to find the point on the box boundary and return
    // true if the point founded is close to origin(start of the curve).
    if (res > 0)
      return isBoundaryPointOnOrigin(point1, point2, dist, numIterations, curveData, eqdsk, box);

    numIterations++;

    // Step : Check if the psi is correct
    double dpsi = eqdsk.getPsiAtPoint(point2) - curveData.psi;
    if (fabs(dpsi) - eqdsk.getPsiTolerance())
    {
      bool adjustedPointToPsi = eqdsk.snapToPsi(point2, curveData.psi);
      assert(adjustedPointToPsi);
    }

    // Step : Calculate distance moved
    if (doesPointHitTheOrigin(point1, point2, startPoint, dist, goal, distNewToStartLast, curveData, eqdsk))
      return true;
    distNewToStartLast = distance2D(point2, curveData.origin);

    if (numIterations > maxIterationsAllowed)
    {
      std::cerr << "In function " << __func__ << ": loop iteration is over 100k. i.e. hard to find next point on a flux curve. \n";
      exit(1); 
    }
    if (m == 0 && dist > goal)
      return true;
    steps--;

    if (steps == 0 && m > 0)
      return true;
  }
}

double getStepToroidalAngle(int numPlanes, int m, int& steps, double stepRadians)
{
  double pi = 3.14159265359;
  double phi = pi*2;

  phi = phi/numPlanes;
  if (m > 0)
    phi = phi/m;

  // calculate step size
  steps = phi/stepRadians;

  if(steps < 1) 
    steps = 1;

  // Calculate step toroidal angle
  // this step_tor_angle might be possible to cause a distance larger than intra_curve_spacing
  double stepToroidalAngle = phi/steps; 
  if(m == 0)
  {
    steps = 10/stepRadians;
    stepToroidalAngle = stepRadians;
  }
  
  return stepToroidalAngle;
}

// Update point pt on the edges of bounding box and return the side of the box its on.
// side = 1 is left, side = 3 is right, side = 2 is bottom, side = 0 is top.
int updatePointOnBoundary(Point& pt, const DomainBox& box)
{
  int side = -1;

  // Step 1. Check if point is on the left of the bounding box.
  // If yes, update the point, and set side to left.
  if (pt.x < box.xMin)
  { 
    side = 1;
    pt.x = box.xMin;
  }

  // Step 2. Check if point is on the right of the bounding box.
  // If yes, update the point, and set side to right.
  if (pt.x > box.xMax)
  {// 
    side = 3;
    pt.x = box.xMax;
  }

  // Step 3. Check if point is on the bottom side of the bounding box.
  // If yes, update the point, and set side to bottom.
  if (pt.y < box.yMin)
  { 
    side = 2;
    pt.y = box.yMin;
  }

  // Step 4. Check if point is on the top side of the bounding box.
  // If yes, update the point, and set side to top.
  if (pt.y > box.yMax)
  {// 
    side = 0;
    pt.y = box.xMax;
  }

  return side;
}

bool isBoundaryPointOnOrigin(Point& pt1, Point& pt2, double& dist, int numIterations, 
                             CurveMetaData& curveData, EqdskData& eqdsk, const DomainBox& box)
{
  int side = updatePointOnBoundary(pt2, box);
  if (side < 0)
    return false;

  bool ok = eqdsk.findNextPsiPointOnBoundary(pt2, curveData.psi, side);
  assert(ok);

  double distPtToPt = distance2D(pt1, pt2);
  dist = dist + distPtToPt;
  pt1 = pt2;
  double distStarttoPt = distance2D(pt1, curveData.origin);
  if (numIterations != 0 && distStarttoPt < distPtToPt)
  {
    curveData.hitOrigin = true;
    return true;      
  }
  
  return false;
}

bool doesPointHitTheOrigin(Point& pt1, Point& pt2, Point startPt, double& dist, double goal,
                           double distNewToStartLast, CurveMetaData& curveData, EqdskData& eqdsk)
{
  double distOldToNew = distance2D(pt1, pt2);
  dist = dist + distOldToNew;
  double distNewToStart = distance2D(pt2, curveData.origin);
  double distSourceToNew = distance2D(startPt, pt2);
  
  pt1 = pt2; // update the point.
 
  if (distNewToStart < distSourceToNew || (distNewToStart < distNewToStartLast &&
      curveData.xPoint && distNewToStart < goal*(1.0+ eqdsk.getSpacingToleranceOptimal())))
  {
    curveData.hitOrigin = true;
    pt1.x = 0.5*(startPt.x + curveData.origin.x);
    pt1.y = 0.5*(startPt.y + curveData.origin.y);
    pt1.z = 0.5*(startPt.z + curveData.origin.z);

    double dpsi = eqdsk.getPsiAtPoint(pt1) - curveData.psi;
    if (fabs(dpsi) - eqdsk.getPsiTolerance())
    {
      bool adjustedPointToPsi = eqdsk.snapToPsi(pt1, curveData.psi);
      assert(adjustedPointToPsi);
    }

    return true;
  } 

  return false;
}

