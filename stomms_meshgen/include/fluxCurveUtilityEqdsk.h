#ifndef FLUXCURVEUTILITYEQDSK_H
#define FLUXCURVEUTILITYEQDSK_H

#include "magneticGeometry.h"

struct curveMetaData{
  Point origin;  // starting point of the flux curve
  double psi;
  bool hitOrigin = false;
  bool hitXPoint = false; 

  // Data from Input (Try to move it to EqdskData and read it here from there)
  int numPlanes = 64;
  double stepRadians = 0.00125;
  double epsilonPsi = 1e-8;
  double spacingToleranceOptimal = 0.5;
};

#endif
