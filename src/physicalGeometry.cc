#include "physicalGeometry.h"

/***********************************************/
// Class: WallCurve
// Base class for wall curves.
/***********************************************/

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
    
    if(!isPointOnCorner(givenPoints[backward], givenPoints[i], givenPoints[forward]))
      continue;  // skip the point

    filteredPoints.push_back(givenPoints[i]);  // Save point to final list of points
  }

  return filteredPoints;
}

// Function to return points on wall curve.
const std::vector <Point>& WallCurve::getPoints() const
{
  return points;
}

/***********************************************/
// Class: WallCurveFromEqdsk
// Class to read wall curve from the eqdsk file.
/***********************************************/
WallCurveFromEqdsk::WallCurveFromEqdsk()
{
  std::cout << "Reading wall curve from eqdsk file ..\n";

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

/***********************************************/
// Class: WallCurveFromFile
// Class to read wall curve from a file that 
// contains the limiter points (R,Z).
/***********************************************/
WallCurveFromFile::WallCurveFromFile(const std::string& wallCurveFile):limiterFile(wallCurveFile)
{
  std::cout << "Reading wall curve from given limiter file ..\n";

  int numPts;
  std::ifstream wallFile(limiterFile);
  
  // Step 1: If can't open the file, exit the program with an error message.
  if (!wallFile.is_open())
  {
    std::cout << "Error opening the limiter input file " << limiterFile << "\n";
    exit(1);
  }

  // Step 2: If the first line is empty, exit with an error message. Else, the value
  // from the first line is stored to numPts
  if (!(wallFile >> numPts))
  {
    std::cout << "The given limiter file seems to be empty" << "\n";
    exit(1);
  }

  // Step 3: Read the values and set them to points
  Point pt = {};  // initializes x,y,z to zero
  int numReadPoints = 0;
  while (wallFile >> pt.x && wallFile >> pt.y)
  {
    if (numReadPoints == numPts)
    {
      std::cout << "Error: Data exceeds number of given points in the file " << limiterFile << "\n";
      exit(1);
    }

    givenPoints.push_back(pt);
    numReadPoints++;
  }

  // Step 4: Get final points (filtered).
  points = filterPoints(givenPoints);
}

/***********************************************/
// Class: PhysicalGeometry
// Class to contain all physical features.
/***********************************************/
PhysicalGeometry::PhysicalGeometry(const args& a)
{
  // Step 1: Read the number of planes
  numPlanes = a.getInputData().pd.planeInput.size();

  // Step 2: If Stellarator, don't do anything (For now, 
  // may need something in future).
  if (a.getReactorType() == ReactorType::Stellarator)
    return;    

  // Step 3: If external limiter file is provided, read the wall curve 
  // from that file, otherwise use wall curve from eqdsk file.
  if (!a.getLimiterFile().empty())
    reactorWall.push_back(WallCurveFromFile(a.getLimiterFile()));
  else
    reactorWall.push_back(WallCurveFromEqdsk());
}

/*
 * Function to return wall curve at specific plane.
 */
const WallCurve& PhysicalGeometry::getWallCurveAtPlane(const int& planeId) const
{
  if (planeId >= numPlanes)
  {
    std::cout << "Max plane index allowed for this case = " << numPlanes-1 << "\n";
    std::cout << "Make sure input plane number index is lower than this number " << "\n";
    std::cout << "Error: Current input index is " << planeId << "\n";
    exit(1);
  }
  
  return reactorWall[planeId];
}

/* 
 * Function to get number of planes from physical geometry.
 */
const int& PhysicalGeometry::getNumPlanes() const
{
  return numPlanes;
}
