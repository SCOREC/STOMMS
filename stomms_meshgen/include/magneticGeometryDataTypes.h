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
    /**
     * To set a model edge to the flux curve.
     * @param ge: model edge.
     */ 
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
    /**
     * Default constructor.
     */ 
    CurveContainer(){};

    /**
     * Constructor to set flux curves already been generated. Takes in set of closed flux curves, separatrices and wall curve.
     * @param closedCurves: a vector of closed flux curves.
     * @param separatrices: a vector of separatrix curves.
     * @param wall: wall curve.
     */ 
    CurveContainer(std::vector <Flux>& closedCurves, std::vector <Flux>& separatrices, WallCurve& wall);

    /**
     * Function to set critical points in the container.
     * @param oPts: vector of O-points.
     * @param xPts: vector of X-points.
     */ 
    void setCriticalPoints(const std::vector <PhysicsPoint>& oPts, const std::vector <PhysicsPoint>& xPts);

    /**
     * Function to set open flux curves in the container.
     * @param openFluxCurves: a vector of open flux curves.
     */ 
    void setOpenCurves(const std::vector <Flux>& openFluxCurves);

    /**
     * Function to set wall curve edges in the container.
     * @param edgesOnWall: model edges on the wall curve.
     */ 
    void setWallEdges(const std::vector <Edge>& edgesOnWall);
     
    /**
     * @return a vector of closed flux curves in the container.
     */ 
    std::vector <Flux>& getCurvesClosed();

    /**
     * @return a vector of separatrix curves in the container.
     */ 
    std::vector <Flux>& getCurvesSeparatrix();

    /**
     * @return a vector of open curves in the container. 
     */ 
    std::vector <Flux>& getCurvesOpen();

    /**
     * @return wall curve.
     */  
    WallCurve& getWallCurve();

    /**
     * @return a vector of model edges on the wall curve.
     */ 
    const std::vector <Edge>& getWallEdges();

    /**
     * @return a vector of O-points.
     */ 
    const std::vector <PhysicsPoint>& getOPoints() const;

    /**
     * @return a vector of X-points.
     */ 
    const std::vector <PhysicsPoint>& getXPoints() const;
  private:
    std::vector <Flux> curvesClosed;  // closed flux curves
    std::vector <Flux> curvesSeparatrix;  // separatrices
    std::vector <Flux> openCurves;  // open flux curves
    std::vector <PhysicsPoint> oPoints;  // O-points
    std::vector <PhysicsPoint> xPoints;  // X-points
    WallCurve wallCurve;  // wall curve
    std::vector <Edge> wallEdges;  // model edges on wall curve
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
  NearVacuum,  // between last flux curve and wall curve
  Private,
  None
};

/**
 * Type of physics fields.
 */
enum class FieldType{
  Psi,
  None
};

/**
 * A class to save data on a grid. We can add different fields to the same Grid.
 */
class GridFieldData{
  public:
    GridFieldData(){};
    GridFieldData(const std::vector <double>& pointsR, const std::vector <double>& pointsZ);
    void setDoubleFieldOnGrid(const std::vector <double>& field, const FieldType& fieldType); 

    const std::vector <double>& getRPoints();
    const std::vector <double>& getZPoints();
    const std::vector <double>& getDoubleFieldData(const FieldType& fieldType);
  private:
    std::vector <double> rPoints;
    std::vector <double> zPoints;
    std::vector <double> psiField;
};

#endif
