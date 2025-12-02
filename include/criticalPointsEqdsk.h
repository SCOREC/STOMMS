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
  // Extend it to 3D if needed in future.
}; 

struct SimplexGrid{
  SimplexGrid(){};
  SimplexGrid(int xResolution, int yResolution, std::array<double,4> box);
  std::vector <Simplex> grid;
};

class SimplexMethod {
  public:
    SimplexMethod(){};
    SimplexMethod(const std::vector <Simplex>& simplexGrid, std::array <double,4> box);

  private:    
    std::vector <PhysicsPoint> candidates;
};

class CriticalPointsEqdsk{
  public:
    CriticalPointsEqdsk(){};
  private:
    // Input information
    std::array <double, 4> domainBox;

    // Information to evaluate first
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;

    std::vector <PhysicsPoint> findMinimumSimplexMethod(); 
};

// Helper Functions:
double getHessianAtPoint(const Point& pt);
#endif
