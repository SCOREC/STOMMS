#ifndef GENFLUXCURVESEQDSK_H
#define GENFLUXCURVESEQDSK_H

#include "fluxCurveUtilityEqdsk.h"

class ClosedFluxCurve{
  public:
    ClosedFluxCurve(const PhysicsPoint& startPt, unsigned int& mySeed, EqdskData& eqdskData);
    Flux& getFluxCurve();
  private:
    Flux f; // flux curve to create
    CurveMetaData curveData;  // metaData for curve.

    // Variables for field following.
    int m = 1;
    bool mChanged = true;
    bool mDecreased = false;
    bool mIncreased = false;

    // Variables for random curve start.
    bool randomGen = false;
    double randomFactor;
    unsigned int& seed;
    bool tagStartingPoint = true;

    // Variables for spacing
    double distanceSet;
    double distance;

    // Curve magnetic properties
    EqdskData& eqdsk;
    double psiNorm;

    // Termination condition
    bool intersect;

    // Internal Functions.
    bool nonFieldFollowingCase(Point& startPoint, Point& nextPoint);
    bool fieldFollowingCase(Point& startPoint, Point& nextPoint);
    void updateM(int& m, bool increase);
};

std::vector <Flux> genClosedFluxCurves(const std::vector <double>& corePsiValues, EqdskData& eqdskData);
std::vector <Flux> genSeparatrixCurves(const std::vector <PhysicsPoint>& xPts, EqdskData& eqdskData, const WallCurve& wall);
#endif
