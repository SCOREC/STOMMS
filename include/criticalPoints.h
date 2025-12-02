#ifndef CRITICALPOINTS_H
#define CRITICALPOINTS_H

#include "modelTopology.h"
#include "physicalGeometry.h"
#include <array>
#include <cmath>

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
std::vector <PhysicsPoint> filterUniquePoints(const std::vector <PhysicsPoint>& candidates);
void checkBounds(Point& pt, std::array<double,4> box);
bool inDomain(const Point& pt, std::array<double,4> box);
#endif
