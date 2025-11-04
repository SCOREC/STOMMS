#ifndef PHYSICALGEOMETRY_H
#define PHYSICALGEOMETRY_H

#include "input.h"
#include "util.h"
#include "modelTopology.h"

// Class for wall curve.
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

// Class to read wall curve from a file that contains the points.
class WallCurveFromFile: public WallCurve{
  public:
    WallCurveFromFile(){};
    WallCurveFromFile(const std::string& wallCurveFile);
  private:
    std::vector <Point> givenPoints;  // raw unfiltered points
    std::string limiterFile;  // file for wall curve points
};

// Class to read wall curve from the eqdsk file.
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

    const WallCurve& getWallCurveAtPlane(const int& planeId) const;
    const int& getNumPlanes() const;
  private:
    std::vector <WallCurve> reactorWall;
    int numPlanes = 1;
};


#endif
