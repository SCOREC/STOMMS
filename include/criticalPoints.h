#ifndef CRITICALPOINTS_H
#define CRITICALPOINTS_H

#include "modelTopology.h"
#include "gfileUtil.h"
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
 * Structs for Simplex method.
 */
struct Simplex{
  Simplex(){};
  Simplex(std::vector <Point> pts);
  Simplex(Point pt1, Point pt2, Point pt3);  
  Point point1;
  Point point2;
  Point point3;
  // Extend it to 3D if needed in future.
}; 

struct SimplexGrid{
  SimplexGrid(){};
  SimplexGrid(int xResolution, int yResolution, std::array<double,4> box);
  std::vector <Simplex> grid;
};

/*
 * Field Types (Not sure if its the best way but going with this for now).
 */
/*enum class FieldType{
  EqdskPsi,
  EqdskGradPsi,
  EqdskDerivativePsi

  // add here any field type for stellarator for function getFieldAtPoint()
}*/



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

class SimplexMethod {
  public:
    SimplexMethod(){};
    SimplexMethod(const std::vector <Simplex>& simplexGrid, std::array <double,4> box);

  private:
    
    std::vector <PhysicsPoint> candidates;
};

class CriticalPointsOnPlane{
  public:
    CriticalPointsOnPlane();
  private:
    // Input information
    std::array <double, 4> domainBox;

    // Information to evaluate first
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;

    
    std::vector <PhysicsPoint> findMinimumSimplexMethod(); 
};

class CriticalPoints{
  public:
    CriticalPoints(){};
  private:
    std::vector <CriticalPointsOnPlane> criticalPointsOnPlane;
};

// Helper Functions:
double getHessianAtPoint(const Point& pt);
void checkBounds(Point& pt, std::array<double,4> box);
#endif
