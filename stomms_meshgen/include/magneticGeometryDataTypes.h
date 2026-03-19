#ifndef MAGNETICGEOMETRYDATATYPES_H
#define MAGNETICGEOMETRYDATATYPES_H

#include <modelTopology.h>
#include <criticalPoints.h>
#include <input.h>
#include <array>

// TO-DO: Make members in Flux and Plane private and use set-get functions.
/**
 * Curves are divided into four basics types. Three of them (closed, open, separatrix) are purely
 * defined by physics and wall curve is the only one that is physical curve.
*/
enum class CurveType{
  Closed,
  Open,
  Separatrix,
  Wall,
  None
};

enum class CurveSubType{
  Closed,
  Open,
  None
};

// General struct for any curve coming out of a separatrix.
struct SeparatrixLeg{
  std::vector <Point> fieldPoints;
  int numXPts = 0;  // number of xPts on the curve
  bool xPtAtStart = false;  // checks if the curve starts with xPt
  bool xPtAtEnd = false;  // checks if the curve ends with xPt
  CurveSubType curveSubType = CurveSubType::None;
};

/**
 * A class to contain the information fo a flux curve.
 */
class Flux{
  public:
    // Set Functions - Try to move data to private in future
    void setSimEdgeToFlux(pGEdge ge);

    int planeNumber = 0;  // plane on which flux curve lies.
    double psiNormOnFlux;  // normalized psi value of flux curve
    std::vector <Edge> edgesOnFlux;
    double nodeSpacingOnFlux;  // node spacing on flux curves

    // Tokamak Part Development
    CurveType curveType = CurveType::None;
    std::vector <SeparatrixLeg> separatrixLegs;
    std::vector <Point> fieldPoints;
    PhysicsPoint xPoint;
};

class CurveContainer{
  public:
    CurveContainer(){};
    CurveContainer(std::vector <Flux>& closedCurves, std::vector <Flux>& separatrices, WallCurve& wall);
    void setCriticalPoints(const std::vector <PhysicsPoint>& oPts, const std::vector <PhysicsPoint>& xPts);
    void setOpenCurves(const std::vector <Flux>& openFluxCurves);
    void setModelVertex(pGVertex gv, PointType pType);
    std::vector <Flux>& getCurvesClosed();
    std::vector <Flux>& getCurvesSeparatrix();
    std::vector <Flux>& getCurvesOpen();
    WallCurve& getWallCurve();
    const std::vector <PhysicsPoint>& getOPoints() const;
    const std::vector <PhysicsPoint>& getXPoints() const;
  private:
    std::vector <Flux> curvesClosed;
    std::vector <Flux> curvesSeparatrix;
    std::vector <Flux> openCurves;
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;
    WallCurve wallCurve;
};

/*
 * The set of surfaces can be classified into seven different types depending on the physics
 * they corresponds to. The physics regions can be increased/decreased as we move forward.
*/ 
enum class FaceType {
  Core,
  ScrapeOffLayer,
  LowFieldSideEdge,
  HighFieldSideEdge,
  LowFieldSideNearVacuum,  // between last flux curve and wall curve
  HighFieldSideNearVacuum,
  Private,
  None
};

#endif
