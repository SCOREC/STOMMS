#ifndef UTIL_H
#define UTIL_H

#include "iostream"
#include "modelTopology.h"
#include <array>
#include <math.h>
#include <cfloat>

// All the utility functions will be written here.

/*************** UTILITY DATA STRUCTURES  ****************************************/
struct DomainBox{
  DomainBox(const double& xMinumum, const double& yMinumum, const double& xMaximum, const double& yMaximum);
  DomainBox(const std::array <double, 4> box);
  double xMin;
  double yMin;
  double xMax;
  double yMax;
};

/*************** UTILITY FUNCTIONS FOR COVERSIONS ********************************/

/**
 * Convert the normalized psi values to actual psi values for a single given value
 * double normPsi (in): normalized psi values.
 * double psiAxis (in): actual psi value at the axis (O-point).
 * double psiCoreBoundary (in): actual psi value at the boundary of core region.
 * returns a actual psi values.
*/
double convertNormToPsi(double normPsi, double psiAxis, double psiCoreBoundary);

/**
 * Convert the normalized psi values to actual psi values. Takes normalized psi values
 * in a vector and return a vector of actual psi values.
 * std::vector <double> normPsi (in): vector of normalized psi values.
 * double psiAxis (in): actual psi value at the axis (O-point).
 * double psiCoreBoundary (in): actual psi value at the boundary of core region.
 * returns a vector of actual psi values.
*/ 
std::vector <double> convertNormToPsiVector(std::vector <double> normPsi, double psiAxis, double psiCoreBoundary);

/*
 * Convert the psi value to normalized psi value.
 * @param psi: psi value to be converted.
 * @param psiAxis: actual psi value at the axis (O-point).
 * @param psiCoreBoundary: actual psi value at the boundary of core region.
 * @return normalized psi value.
*/
double convertPsiToNorm(double psi, double psiAxis, double psiCoreBoundary);

/*************** UTILITY FUNCTIONS FOR GEOMETRY ********************************/

/**
 * Function to check if a point is on a straight line or it makes a corner on the
 * curve. Need two intersecting lines of the curve on that point to check.
 * Point& pt1 (in): start point of the line one.
 * Point& pt2 (in): point to check (end point of line one and start of line two).
 * Point& pt1 (in): end  point of the line two.
 * return true if point makes a corner, otherwise false if its on a line.
 */
bool isPointOnCorner(const Point& pt1, const Point& pt2, const Point& pt3);

/** \fn
 * Function to compare two points.
 * const Point& pt1 (in): first test point.
 * const Point& pt2 (in): second test point.
 * returns true if points are same, else false.
 */

bool arePointsSame(const Point& pt1, const Point& pt2);

/**
 * Given two endpoints of a line segment (p1 ... p2), and a point (testPoint),
 * evaluate if the testPoint is on left side or right side of line segment.
 * const Point& pt1 (in): end point 1 of line segment.
 * const Point& pt2 (in): end point 2 of line segment.
 * const Point& testPoint (in): point to bes tested.
 * returns true if point is on left side, else false.
 */
int isLeft(const Point& pt1, const Point& pt2, const Point& testPoint);

/**
 * Winding number test for a point in polygon. 
 * const Point& pt (in): test point.
 * const std::vector <Point>& curve (in): vector of points defining a closed curve (polygon).
 * Returs 0 if point is outside the polygon. 
 */
int windingNumberPolygonTest(const Point& pt, const std::vector <Point>& curve);

/**
 * Function to calculate 2D distance between two points.
 * @param pt1: first point.
 * @param pt2: second point.
 * @return 2D distance.
 */
double distance2D(const Point& pt1, const Point& pt2);

/**
 * Function to get the parametric coordinate of a point on a line segment.
 * @param pt1: start point of the line segment.
 * @param pt2: end point of the line segment. 
 * @param pt: point for which parametric value is needed.
 * @return parametric value of point pt on line segment(pt1---pt2).
 */
double getParamatricCoordinate(const Point& pt1, const Point& pt2, const Point& pt);

/**
 * Function to get the distance from a point to a line segment.
 * @param pt1: start point of the line segment.
 * @param pt2: end point of the line segment. 
 * @param pt: point for which distance from the line is needed.
 * @return distance from point pt to line segment(pt1---pt2).
 */
double distanceLineToPoint(const Point& pt1, const Point& pt2, const Point& pt);

bool curveOrientation(const std::vector <Point>& curvePts);
std::array <double, 4> getCurveBounds(const std::vector <Point>& curvePts);
bool isPtOnModelEdge(const Point& checkPt, const pGEdge& ge);
bool isPtOnCurve(const Point& pt, const std::vector <pGEdge>& edgesOnCurve);
#endif
