#ifndef FLUXCURVEUTILITYEQDSK_H
#define FLUXCURVEUTILITYEQDSK_H

#include "magneticGeometry.h"

/**
 * A struct to hold the curve meta data for curve construction.
 * It includes starting point of curve (origin), psi, if it has
 * an xPoint or not. It gets updated while curve tracing such as 
 * hitOrigin , hitXPoint etc.
 */
struct CurveMetaData{
  Point origin;  // starting point of the flux curve
  double psi;
  bool hitOrigin = false;
  bool hitXPoint = false;
  bool xPoint = false;

  std::vector <PhysicsPoint> xPoints; 
};

/**
 * Given a point on curve, and properties of magnetic field along with the curve meta deta, find next point on the poloidal curve.
 * Its not field following. For field-following curve tracing, function is findNextFieldFollowingPoint.
 * @param startPoint: current point on the curve (last successful traced point).
 * @param nextPoint: the next traced point.
 * @param lengthPoloidalGoal: local 2D distance from start point to the next finding point.
 * @param oPoint: magnetic axis (oPoint).
 * @param curveData: curve meta data containing psi values etc.
 * @param eqdsk: Eqdsk class for handling magnetic field related operations/queries/parameters.
 * @param box: bounding box of the domain.
 * @return true if the next point is found.
 */
bool findNextPoint(Point& startPoint, Point& nextPoint, double& lengthPoloidalGoal, const PhysicsPoint& oPoint, CurveMetaData& curveData, EqdskData eqdsk, const DomainBox& box);

/**
 * Given a point on curve, and properties of magnetic field along with the curve meta deta, find next point on the poloidal curve.
 * Its field following, and traces the point along the field line.
 * @param startPoint: current point on the curve (last successful traced point).
 * @param nextPoint: the next traced point.
 * @param dist: local 2D distance from start point to the next finding point.
 * @param m: a division parameter for steppings in toroidal direction.
 * @param curveData: curve meta data containing psi values etc.
 * @param eqdsk: Eqdsk class for handling magnetic field related operations/queries/parameters.
 * @param box: bounding box of the domain.
 * @return true if the next point is found.
 */
bool findNextFieldFollowingPoint(Point& startPoint, Point& nextPoint, double& dist, int m, CurveMetaData& curveData, EqdskData eqdsk, const DomainBox& box);

/**
 * To find the toroidal step size for rk4 method.
 * @param numPlanes: number of target poloidal planes.
 * @param m: a division parameter for steppings in toroidal direction.
 * @param steps: number of rk4 steps.
 * @param stepRadians: change in phi in radians of each rk4 step taken when 
 *                     moving a point along a psi curve (user input).
 * @return toroidal step size
 */
double getStepToroidalAngle(int numPlanes, int m, int& steps, double stepRadians);

/**
 * To find normalized toroidal step for rk4 method.
 * @param oPoint: magnetic axis (oPoint).
 * @param goal: local 2D distance from start point to the next finding point.
 * @param numPlanes: number of target poloidal planes.
 * @return 
 */
double getStepToroidalUnit(const PhysicsPoint& oPoint, double goal, int numPlanes);

/**
 * If a point is outside the bounding box, readjust it on the boundary.
 * @param pt: pt to be adjusted.
 * @param box: bounding box of the domain.
 * returns the side of the box on which point lies. <br>
 * side = 1 is left, side = 3 is right, side = 2 is bottom, side = 0 is top.
 */
int updatePointOnBoundary(Point& pt, const DomainBox& box);

/**
 * Check if the point adjusted on the bounding box, hits the origin (starting point) of curve or not.
 * @param pt1: point to be updated.
 * @param pt2: point to be checked and adjusted.
 * @param dist: local 2D distance from start point to the next finding point.
 * @param numIterations: current number of iterations to test a return condition.
 * @param curveData: curve meta data containing psi values etc.
 * @param eqdsk: Eqdsk class for handling magnetic field related operations/queries/parameters.
 * @param box: bounding box of the domain.
 * @return true if point hits the starting point.
 */
bool isBoundaryPointOnOrigin(Point& pt1, Point& pt2, double& dist, int numIterations,
                             CurveMetaData& curveData, EqdskData& eqdsk, const DomainBox& box);

/**
 * Check if the next traced points, hits the origin (starting point) of curve or not.
 * @param pt1: point to be updated.
 * @param pt2: point to be checked and adjusted.
 * @param startPt: starting point of the curve for the test.
 * @param dist: local 2D distance from start point to the next finding point.
 * @param distNewToStartLast: distance between the last guessed pt (pt2) in last 
 *                            iteration and starting point of curve. 
 * @param curveData: curve meta data containing psi values etc.
 * @param eqdsk: Eqdsk class for handling magnetic field related operations/queries/parameters.
 * @return true if point hits the starting point.
 */
bool doesPointHitTheOrigin(Point& pt1, Point& pt2, Point startPt, double goal, double distNewToStartLast, 
                           CurveMetaData& curveData, EqdskData& eqdsk);
#endif
