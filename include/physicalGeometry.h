#ifndef PHYSICALGEOMETRY_H
#define PHYSICALGEOMETRY_H

#include "input.h"
#include "util.h"
#include "modelTopology.h"

/* 
 * Base Class for wall curve.
 */
class WallCurve{
  public:

    /*
     * Function to return points on wall curve.
     */ 
    const std::vector <Point>& getPoints() const; 
  protected:
   // Data members
   std::vector <Point> points;

   // Member Functions
   /*
    * Function to filter out the points that are not needed in the final
    * definition of the curve. Right now only filters out points that are
    * on a straight line.
    * std::vector <Point>& givenPoints (in): Vector of given set of points.
    * returns the filtered points.
    */ 
   std::vector <Point> filterPoints(std::vector <Point>& givenPoints);
};

/* 
 * Class to read wall curve from a file that contains the points.
 * Derived class 1 for WallCurve.
 */
class WallCurveFromFile: public WallCurve{
  public:
    WallCurveFromFile(){};
    WallCurveFromFile(const std::string& wallCurveFile);
  private:
    std::vector <Point> givenPoints;  // raw unfiltered points
    std::string limiterFile;  // file for wall curve points
};

/* 
 * Class to read wall curve from the eqdsk file.
 * Derived class 2 for WallCurve.
 */
class WallCurveFromEqdsk: public WallCurve{
  public:
    WallCurveFromEqdsk();
  private:
    std::vector <Point> givenPoints;  // raw unfiltered points
};

// Class to define the physical geometry of the reactor. Right now
// only physical feature is wall curve. Add more here if needed. 
class PhysicalGeometry{
  public:
    PhysicalGeometry(){};
    PhysicalGeometry(const args& a);

    /*
     * Function to return wall curve at specific plane.
     * const int& planeId (in): number of the plane on which wall curve is needed.
     */ 
    const WallCurve& getWallCurveAtPlane(const int& planeId) const;

    /*
     * Function to get number of planes from physical geometry.
     */ 
    const int& getNumPlanes() const;
  private:
    std::vector <WallCurve> reactorWall;  // Vector of wall curves. One for each plane.
    int numPlanes = 1;  // set number of planes (minimum = 1).
};


#endif
