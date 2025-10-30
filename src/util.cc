#include <util.h>

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

// Using cross product, check if a point is on a straight line or on a corner.
bool isPointOnCorner(Point* pt1, Point* pt2, Point* pt3)
{
  // Step 1: Get the vectors between pt1 and pt2, and between pt2 and pt3.
  std::array<double,2> vec1 = {pt1->x - pt2->x, pt1->y - pt2->y};
  std::array<double,2> vec2 = {pt3->x - pt2->x, pt3->y - pt2->y};

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

