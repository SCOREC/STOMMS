#include "physicalGeometry.h"

// Function to filter out the points that are not needed in the final definition 
// of the curve.  Right now only filters out points that are on a straight line.
std::vector <Point> WallCurve::filterPoints(std::vector <Point>& givenPoints)
{
  std::vector <Point> filteredPoints; 
  // Step 1: Push the first point to the final vector.
  filteredPoints.push_back(givenPoints[0]);

  // Step 2: Iterate over rest of them and if they are on a 
  // straight line, discard them.
  int numPts = givenPoints.size();
  for (int i = 1; i < numPts; i++)
  {
    // Step 2.1: If i is current point, we need one backward 
    // point and one forward point
    int backward = i-1;  
    int forward = (i+1)%numPts;
    
    if(!isPointOnCorner(&givenPoints[backward], &givenPoints[i], &givenPoints[forward]))
      continue;  // skip the point

    filteredPoints.push_back(givenPoints[i]);  // Save point to final list of points
  }

  return filteredPoints;
}

// Class to read wall curve from the eqdsk file.
WallCurveFromEqdsk::WallCurveFromEqdsk()
{
  // Step 1: Get the wall curve points in the g-eqdsk file.
  // First get number of points and then points.
  int numPts = 0;
  get_num_bd_pts_(&numPts);  // Number of points
  std::vector<double> xVector(numPts), yVector(numPts);
  get_bd_pts_(xVector.data(), yVector.data(), &numPts);

  // Step 2: Read these points to the local vector.
  for (int i = 0; i < numPts; i++)
  {
    Point pt = {};  // initializes x,y,z to zero
    pt.x = xVector[i];
    pt.y = yVector[i];
    givenPoints.push_back(pt);
  }
  
  // Step 3: Get final points (filtered).
  points = filterPoints(givenPoints);

}

// PhysicalGeometryClass
PhysicalGeometry::PhysicalGeometry(const args& a)
{
  reactorWall = WallCurveFromEqdsk();
}
