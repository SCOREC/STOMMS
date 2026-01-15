#ifndef FLUXCURVEUTILITYEQDSK_H
#define FLUXCURVEUTILITYEQDSK_H

struct curveMetaData{
  Point origin;  // starting point of the flux curve
  double psi;
  double desiredSpacing;
  bool hitOrigin;
  bool hitXPoint;
};

#endif
