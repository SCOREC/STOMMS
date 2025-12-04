#ifndef CRITICALPOINTSEQDSK_H
#define CRITICALPOINTSEQDSK_H

#include "criticalPoints.h"
#include "gfileUtil.h"

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

  // Accessing simplex in array form is desireable at some points.
  std::array<std::array<double, 2>, 3> points;
  // Extend it to 3D if needed in future.
}; 

struct SimplexGrid{
  SimplexGrid(){};
  SimplexGrid(int xResolution, int yResolution, std::array<double,4> box);
  std::vector <Simplex> simplexVec;
};

/**********************************************************/
// Class SimplexMethod 
// 2D Downhill Simplex Method for finding  critical points
// Sources modified from Numerical Recipe Ch. 10.4
/**********************************************************/
class SimplexMethod {
  public:
    SimplexMethod(){};
    SimplexMethod(const SimplexGrid& simplexGrid, std::array <double,4> box);

    const std::vector <Point>& getCandidates();
  private:
    // Input information
    std::array <double, 4> domainBox;
    SimplexGrid grid;

    // Output information    
    std::vector <Point> candidates;

    // Member functions:
    double evaluateTrialPoint(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, std::array<double,2>& pSum, int ihi, double factor);
    int evaluateMinimum(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, const double& fToleranceAbs, 
                        const double& fToleranceRel, int& nFunc);
};

class CriticalPointsEqdsk{
  public:
    CriticalPointsEqdsk(){};
    CriticalPointsEqdsk(const WallCurve& wall);
    const std::vector <PhysicsPoint>& getOPoints();
    const std::vector <PhysicsPoint>& getXPoints();
  private:
    // Input information
    std::array <double, 4> domainBox;
    WallCurve wallCurve;
    std::vector <Point> wallPoints;

    // Information to evaluate first
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;

    std::vector <Point> findMinimumSimplexMethod();
    int findMinimumNewtonMethod(const Point& initialGuess, Point& finalPosition, const std::array<double,4> domain); 
};

// Helper Functions:
PointType getPointType(const Point& pt);
#endif
