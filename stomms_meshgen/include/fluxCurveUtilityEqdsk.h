#ifndef FLUXCURVEUTILITYEQDSK_H
#define FLUXCURVEUTILITYEQDSK_H

#include "magneticGeometry.h"

struct CurveMetaData{
  Point origin;  // starting point of the flux curve
  double psi;
  bool hitOrigin = false;
  bool hitXPoint = false;
  bool xPoint = false;

  std::vector <PhysicsPoint> xPoints; 
};

bool findNextFieldFollowingPoint(Point& startPoint, Point& newPoint, double& dist, int m, CurveMetaData& curveData, EqdskData eqdsk, const DomainBox& box);
double getStepToroidalAngle(int numPlanes, int m, int& steps, double stepRadians);
int updatePointOnBoundary(Point& pt, const DomainBox& box);
bool isBoundaryPointOnOrigin(Point& pt1, Point& pt2, double& dist, int numIterations,
                             CurveMetaData& curveData, EqdskData& eqdsk, const DomainBox& box);
bool doesPointHitTheOrigin(Point& pt1, Point& pt2, Point startPt, double& dist, double goal,
                           double distNewToStartLast, CurveMetaData& curveData, EqdskData& eqdsk);
#endif
