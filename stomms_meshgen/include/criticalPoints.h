#ifndef CRITICALPOINTS_H
#define CRITICALPOINTS_H

#include "modelTopology.h"
#include "physicalGeometry.h"
#include <array>
#include <cmath>
#include <algorithm>

/**
 * Basic model vertices types.
 */
enum class PhysicsPointType{
  OPoint,
  XPoint,
  None
};

/**
 * Physics point contains both the physical coordinate (Point) and physics
 * properties such as point type, psi values. Add more point properties here
 * if needed.
 */
class PhysicsPoint{
  public:
    PhysicsPoint(){};

    /**
     * Constructor.
     * @param point: an object of Point containing the physical coordinates.
     * @param psiAtPoint: psi value at the point.
     * @param pType: physics type of the point (OPoint, XPoint, None).
     */
    PhysicsPoint(const Point& point, const double& psiAtPoint, const PhysicsPointType pType);

    /**
     * Function to get physical coordinates of a physics point.
     */
    const Point& getPoint() const;

    /**
     * Function to get psi value of a physics point.
     */
    const double& getPsi() const;

    /**
     * Function to get point type.
     */
    const PhysicsPointType& getPointType() const;
  private:
    Point pt;  // Point in physical space
    double psi;  // associated psi value
    PhysicsPointType pointType = PhysicsPointType::None;  // initialize point with type None.
};

// Helper Functions:

/**
 * If the given Point pt is slightly outside the domain box due to 
 * floating point inconsistencies, readjust it to the boundary of box.
 * @param pt: point to be tested and adjusted.
 * @param box: domain box boundary defined by an array of size 4. 
 */
void checkBounds(Point& pt, const std::array<double,4>& box);

/**
 * Check if the given point pt is inside or outside of the domain box.
 * @param pt: point to be tested.
 * @param box: domain box boundary defined by an array of size 4.
 * @return true if the point is inside the box, false otherwise.
 */
bool inDomain(const Point& pt, const std::array<double,4>& box);

/**
 * Given a vector of points, filter the unique points and get rid of duplicates.
 * @param candidates: a vector on input points.
 * @return a vector of filtered points with no duplicates.
 */
std::vector <Point> filterUniquePoints(const std::vector <Point>& candidates);

/**
 * Given a vector of points, filter the points that are outside the wall curve.
 * @param candidates: a vector of input points.
 * @param wall: a vector of points on wall curve.
 */

void filterOutsideTheWallPoints(std::vector <Point>& candidates, const std::vector <Point>& wall);

/**
 *  Given a vector of critical points, print them out for execution report.
 *  @param criticalPoints: a vector of critical points.
 */
void printCriticalPoints(const std::vector <PhysicsPoint>& criticalPoints);

/**
 * Given two points, compare their psi values. If psi of pt1 is less than or equal to
 * psi of pt 2, return true.
 * @param pt1: first physics point.
 * @param pt2: second physics point.
 * @return true if psi value of pt1 is less or equal to psi value of pt2. Else, false.
 */
bool comparePhysicsPoints(const PhysicsPoint& pt1, const PhysicsPoint& pt2);
#endif
