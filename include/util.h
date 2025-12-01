#ifndef UTIL_H
#define UTIL_H

#include "iostream"
#include "modelTopology.h"
#include <array>
#include <math.h>

// All the utility functions will be written here.

/*************** UTILITY FUNCTIONS FOR COVERSIONS ********************************/

/*
 * Convert the normalized psi values to actual psi values for a single given value
 * double normPsi (in): normalized psi values.
 * double psiAxis (in): actual psi value at the axis (O-point).
 * double psiLCF (in): actual psi value at the last closed curve.
 * returns a actual psi values.
*/
double convertNormToPsi(double normPsi, double psiAxis, double psiLCF);

/*
 * Convert the normalized psi values to actual psi values. Takes normalized psi values
 * in a vector and return a vector of actual psi values.
 * std::vector <double> normPsi (in): vector of normalized psi values.
 * double psiAxis (in): actual psi value at the axis (O-point).
 * double psiLCF (in): actual psi value at the last closed curve.
 * returns a vector of actual psi values.
*/ 
std::vector <double> convertNormToPsiVector(std::vector <double> normPsi, double psiAxis, double psiLCF);

/*************** UTILITY FUNCTIONS FOR GEOMETRY ********************************/

/*
 * Function to check if a point is on a straight line or it makes a corner on the
 * curve. Need two intersecting lines of the curve on that point to check.
 * Point& pt1 (in): start point of the line one.
 * Point& pt2 (in): point to check (end point of line one and start of line two).
 * Point& pt1 (in): end  point of the line two.
 * return true if point makes a corner, otherwise false if its on a line.
 */
bool isPointOnCorner(const Point& pt1, const Point& pt2, const Point& pt3);

bool arePointsSame(const Point& pt1, const Point& pt2);

int isLeft(const Point& pt1, const Point& pt2, const Point& testPoint);
int windingNumberPolygonTest(const Point& pt, const std::vector <Point>& curve);

#endif
