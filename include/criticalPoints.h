#ifndef CRITICALPOINTS_H
#define CRITICALPOINTS_H

#include "modelTopology.h"
#include "physicalGeometry.h"
#include <array>
#include <cmath>
#include <algorithm>

/*
 * Basic model vertices types.
 */
enum class PointType{
  OPoint,
  XPoint,
  None
};

/*
 * Physics point contains both the physical coordinate (Point) and physics
 * properties such as point type, psi values. Add more point properties here
 * if needed.
 */
class PhysicsPoint{
  public:
    PhysicsPoint(){};

    /*
     * Constructor.
     * const Point& point (in): physical coordinates of the point defined in Point.
     * const double& psiAtPoint (in): psi value at the point.
     * const PointType pType (in): physics type of the point (oPoint, xPoint).
     */
    PhysicsPoint(const Point& point, const double& psiAtPoint, const PointType pType);

    /*
     * Function to get physical coordinates of a point.
     */
    const Point& getPoint() const;

    /*
     * Function to get psi value at a point.
     */
    const double& getPsi() const;

    /*
     * Function to get point type.
     */
    const PointType& getPointType() const;
  private:
    Point pt;  // Point in physical space
    double psi;  // associated psi value
    PointType pointType = PointType::None;  // initialize point with type None.
};

// Helper Functions:

/*
 * If the given Point pt is slightly outside the domain box due to 
 * floating point inconsistencies, readjust it to the boundary of box.
 * Point& pt (in, out): point to be tested and adjusted.
 * const std::array<double,4>& box (in): domain box boundary. 
 */
void checkBounds(Point& pt, const std::array<double,4>& box);

/*
 * Check if the given point pt is inside or outside of the domain box.
 * const Point& pt (in): point to be tested.
 * const std::array<double,4>& box (in): domain box boundary.
 * returns true if the point is inside the box.
 */
bool inDomain(const Point& pt, const std::array<double,4>& box);

/*
 * Given a vector of points, filter the unique points and get rid of duplicates.
 * const std::vector <Point>& candidates (in): a vector on input points.
 * returns a vector of filtered points with no duplicates.
 */
std::vector <Point> filterUniquePoints(const std::vector <Point>& candidates);

/*
 *  Given a vector of critical points, print them out for execution report.
 *  const std::vector <PhysicsPoint>& criticalPoints (in) : a vector of critical points.
 */
void printCriticalPoints(const std::vector <PhysicsPoint>& criticalPoints);

/*
 * Given two points, compare their pis values. If psi of pt1 is less than or equal to
 * psi of pt 2, return true.
 * const PhysicsPoint& pt1 (in): first point
 * const PhysicsPoint& pt2 (in): second point
 */
bool comparePhysicsPoints(const PhysicsPoint& pt1, const PhysicsPoint& pt2);
#endif
