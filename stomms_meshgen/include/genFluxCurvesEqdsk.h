#ifndef GENFLUXCURVESEQDSK_H
#define GENFLUXCURVESEQDSK_H

#include "fluxCurveUtilityEqdsk.h"
#include "modelDataEqdsk.h"

class ClosedFluxCurve{
  public:
    ClosedFluxCurve(const PhysicsPoint& startPt, unsigned int& mySeed, const PhysicsPoint& oPoint, 
                    EqdskData& eqdskData, const PlaneMetaData& planeMetaData);
    Flux& getFluxCurve();
  private:
    Flux f; // flux curve to create
    CurveMetaData curveData;  // metaData for curve.
    PlaneMetaData pMetaData;  //plane meta data.

    // Variables for field following.
    int m = 1;
    bool mChanged = true;
    bool mDecreased = false;
    bool mIncreased = false;

    // Variables for random curve start.
    bool randomGen = false;
    double randomFactor;
    unsigned int seed;
    bool tagStartingPoint = true;

    // Variables for spacing
    double distanceSet;
    double distance;

    // Curve magnetic properties
    EqdskData& eqdsk;
    double psiNorm;
    PhysicsPoint magneticAxis;

    // Termination condition
    bool intersect;

    // Internal Functions.
    bool nonFieldFollowingCase(Point& startPoint, Point& nextPoint);
    bool fieldFollowingCase(Point& startPoint, Point& nextPoint);
    void updateM(int& m, bool increase);
};


class SeparatrixCurve{
  public:
    SeparatrixCurve(const PhysicsPoint& xPt, std::vector <Point> startPoints, EqdskData& eqdskData, const WallCurve& wall, const PlaneMetaData& planeMetaData);
    const std::vector <Flux>& getFluxCurves() const;
  private:
    std::vector <Flux> sepCurves; // flux curves to create
    CurveMetaData curveData;  // metaData for curve.
    PlaneMetaData pMetaData;  //plane meta data.
    WallCurve wall;
    std::vector <Point> startPts;
    std::vector <Point> pushedPoints;    

    // Variables for field following.
    int m = 1;
    bool mChanged = true;
    bool mDecreased = false;
    bool mIncreased = false;

    // Variables for spacing
    double distanceSet;
    double distance;

    // Curve magnetic properties
    EqdskData& eqdsk;
    double psiNorm;
    Point xPoint;
    double psi;

    // Termination condition
    bool intersect;

    // Internal Functions.
    void getSeparatrixLeg(const Point& point, SeparatrixLeg& leg);
    std::vector <Flux> mergeSeparatrixLegs(std::vector <SeparatrixLeg>& separatrixLegs);
    void updateM(int& m, bool increase);
};


class OpenFluxCurve{
  public:
    OpenFluxCurve(std::vector <PhysicsPoint> startPoints, int startPtIndex, EqdskData& eqdskData, 
                  const WallCurve& wallCurve, const PlaneMetaData& planeMetaData);
    const std::vector <Flux>& getFluxCurves() const;
  private:
    std::vector <Flux> openCurves; // flux curve to create
    CurveMetaData curveData;  // metaData for curve.
    PlaneMetaData pMetaData;  //plane meta data.
    WallCurve wall;
    std::vector <PhysicsPoint> startPts;
    int index;  // Index of the start point from the vector to start

    // Variables for field following.
    int m = 1;
    bool mChanged = true;
    bool mDecreased = false;
    bool mIncreased = false;

    // Variables for spacing
    double distanceSet;
    double distance;

    // Curve magnetic properties
    EqdskData& eqdsk;
    double psiNorm;
    double psi;    

    // Termination condition
    bool intersect;

    // Internal Functions.
    void updateM(int& m, bool increase);
};

std::vector <Flux> genClosedFluxCurves(const std::vector <double>& corePsiValues, const PhysicsPoint& oPoint, EqdskData& eqdskData, const PlaneMetaData& planeMetaData);
std::vector <Flux> genSeparatrixCurves(const std::vector <PhysicsPoint>& xPts, EqdskData& eqdskData, const WallCurve& wall, const PlaneMetaData& planeMetaData);
std::vector <Flux> genOpenFluxCurves(pGModel& model, EqdskData& eqdskData, const WallCurve& wall, const PlaneMetaData& planeMetaData);
#endif
