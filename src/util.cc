#include <util.h>

/***********************************************/
// UTILITY FUNCTIONS FOR COVERSIONS
/***********************************************/
// Convert the normalized psi values to actual psi values for a single given value.
double convertNormToPsi(double normPsi, double psiAxis, double psiLCF)
{
  double psi = normPsi*(psiLCF - psiAxis) + psiAxis;
  return psi; 
}

// Convert the normalized psi values to actual psi values for the full vector. 
std::vector <double> convertNormToPsiVector(std::vector <double> normPsi, double psiAxis, double psiLCF)
{
  std::vector <double> psiValues;
  for (int i = 0; i < normPsi.size(); i++)
  {
    double psiNorm = normPsi[i];  // Normalized psi value
    double psi = psiNorm*(psiLCF - psiAxis) + psiAxis;
    psiValues.push_back(psi);
  }
  return psiValues;
}

/***********************************************/
// UTILITY FUNCTIONS FOR GEOMETRY 
/***********************************************/
// Using cross product, check if a point is on a straight line or on a corner.
bool isPointOnCorner(const Point& pt1, const Point& pt2, const Point& pt3)
{
  // Step 1: Get the vectors between pt1 and pt2, and between pt2 and pt3.
  std::array<double,2> vec1 = {pt1.x - pt2.x, pt1.y - pt2.y};
  std::array<double,2> vec2 = {pt3.x - pt2.x, pt3.y - pt2.y};

  // Step 2: Get the length of vectors between pt1 and pt2, and between pt2 and pt3.
  double len1 = sqrt(vec1[0]*vec1[0] + vec1[1]*vec1[1]);
  double len2 = sqrt(vec2[0]*vec2[0] + vec2[1]*vec2[1]);

  // Compare the cross product with tolerance, and if its relatively close to zero, its
  // on straight line, else its a corner.
  // Sinθ = (vector1 x vector2)/(|length1||length2|)
  double tolerance = 1e-3;
  if(fabs((vec1[0]*vec2[1]-vec1[1]*vec2[0])/(len1*len2)) > tolerance)
    return true;

  return false;
}

// Function to compare two points. 
// returns true if points are same, else false.
bool arePointsSame(const Point& pt1, const Point& pt2)
{
  double dx = fabs(pt1.x - pt2.x);
  double dy = fabs(pt1.y - pt2.y);
  double dz = fabs(pt1.z - pt2.z);

  if (dx < 1e-5 && dy < 1e-5 && dz < 1e-5)
    return true;
  
  return false;
}

// Given two endpoints of a line segment (p1 ... p2), and a point (testPoint),
// evaluate if the testPoint is on left side or right side of line segment.
// This is done using the orientation (clockwise vs counter clockwise).
int isLeft(const Point& pt1, const Point& pt2, const Point& testPoint)
{
  // Step 1: Define two vector. First for the line segment (edgeVector) and second for the
  // segment connecting one end point of line and test point.
  std::array <double,2> edgeVector = {pt2.x - pt1.x, pt2.y - pt1.y};
  std::array <double,2> pointToLineVector = {testPoint.x - pt1.x, testPoint.y - pt1.y};

  // Step 2: Evaluate cross product.
  double crossProduct = edgeVector[0]*pointToLineVector[1] - pointToLineVector[0]*edgeVector[1];

  // Step 3: Make the decison based on cross product.
  const double tolerance = 1e-12; 
  if (crossProduct > tolerance)
    return 1;  // is left (counter clock wise)
  else if (crossProduct < tolerance)
    return -1;  // is right (clock wise)
  else
    return 0;  // on the line (not necessarily line segment).
}

// Winding Number Polygon Test
int windingNumberPolygonTest(const Point& pt, const std::vector <Point>& curve)
{
  // Step 1: Make sure curve is closed (first and last point is same)
  assert (curve.size() >= 4);  // Minimum 4 points required

  if (!arePointsSame(curve[0], curve[curve.size()-1]))
  {
    std::cout << "ERROR: First and last points are not identical. Curve is not closed\n";
    std::cout << "Make sure the curve given to Winding Number Polygon Test is closed\n";
    exit(0);
  }

  // Step 2: Iterate over the edges and increase/decrease winding number based on the
  // outcome of each edge. Winding number starts with zero.
  int windingNumber = 0;

  for (int i = 0; i < curve.size()-1; i++)
  {
    // pt1 and pt2 define two endpoints of the line.
    Point pt1 = curve[i];
    Point pt2 = curve[i+1];

    if (pt1.y <= pt.y) 
    {
      if (pt2.y > pt.y)  // upward crossing
        if (isLeft(pt1, pt2, pt) > 0)  // pt is left of the edge
          windingNumber++; 
    }
    else  // pt1.y > pt.y
    {
      if (pt2.y <= pt.y)  // downward crossing
        if (isLeft(pt1, pt2, pt) < 0)  // pt is right of the edge
          windingNumber--;
    }
  }

  return windingNumber;
}

