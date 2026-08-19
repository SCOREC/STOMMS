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

/**
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
 * Currently supports reading grid data from eqdsk/eqd files.
*/
class GridFieldData{
  public:
    /**
     * Default constructor.
     */ 
    GridFieldData(){};
    
    /**
     * Constructor to create a physical grid. It adds no field data to grid.
     * @param pointsR: Coordinates of the points in the R direction.
     * @param pointsZ: Coordinates of the points in the Z direction.
     */ 
    GridFieldData(const std::vector <double>& pointsR, const std::vector <double>& pointsZ);

    /**
     * Function to set a field of type double on the grid.
     * @param field: a vector of the field values on the grid.
     * @param fieldType: field type. See class FieldType for details.
     */  
    void setDoubleFieldOnGrid(const std::vector <double>& field, const FieldType& fieldType); 

    /**
     * Function to set array of psi values from the magnetic field source file.
     * @param psi: a vector of psi values.
     */ 
    void setPsiArray(const std::vector <double>& psi);

    /**
     * Function to set an array of poloidal current values from the magnetic field source file.
     * @param poloidalCurrent: a vector of poloidal current values.
     */ 
    void setPoloidalCurrentArray(const std::vector <double>& poloidalCurrent);

    /**
     * Function to set raw limiter data (wall curve) to the grid data.
     * @param rLim: R coordinates of the limiter points.
     * @param zLim: Z coordinates of the limiter points.
     */ 
    void setLimiter(const std::vector <double>& rLim, const std::vector <double>& zLim);

    /**
     * Function to set plasma boundary (separatrix) data to the grid data.
     * @param rBdry: R coordinates of the poloidal boundary.
     * @param zBdry: Z coordinates of the poloidal boundary. 
     */ 
    void setPlasmaBoundary(const std::vector <double>& rBdry, const std::vector <double>& zBdry);

    /**
     * Function to set the domain box to grid data.
     * @param box: A vector of size = 4 containing rMin, yMin, rMax, yMax.
     */   
    void setDomainBox(const std::vector <double>& box);

    /**
     * Function to set psi spline coefficients to the grid data.
     * @param coefficients: a vector containing the coefficients.
     * @param shape: a vector of size 3 defining the shape of coefficients array.
     * coefficients.size() == shape[0]*shape[1]*shape[2]
     */  
    void setPsiSpline(std::vector <double>& coefficeints, std::vector <int>& shape);

    /**
     * Function to set poloidal current spline coefficients to the grid data.
     * @param coefficients: a vector containing the coefficients.
     * @param shape: a vector of size 2 defining the shape of coefficients array.
     * coefficients.size() == shape[0]*shape[1]
     */
    void setCurrentSpline(std::vector <double>& coefficeints, std::vector <int>& shape);
  
    /**
     * Function to set psi values at Chebyshev points of the cells in the grid.
     * Needed to evaluate bicubic spline coefficients in XGC.
     * @param psiValuesAtPoints: psi values on each point of each cell.
     * psiValuesAtPoints size should be (4*4)*(numPointsR-1)*(numPointsZ-1)
     */
    void setPsiAtChebyshevPoints(const std::vector <double>& psiValuesAtPoints);

   /**
    * Function to set psi bicubic spline coefficients for each grid cell.
    * @param biCubicSplineCoefficientsGrid: coefficients on each point of each cell.
    * biCubicSplineCoefficientsGrid size should be (4*4)*(numPointsR-1)*(numPointsZ-1).
    */ 
   void setBiCubicSplineCoefficients(const std::vector <double>& biCubicSplineCoefficientsGrid);

    /**
     * Function to return r grid points.
     */ 
    const std::vector <double>& getRPoints() const;

    /**
     * Function to return z grid points.
     */ 
    const std::vector <double>& getZPoints() const;

    /**
     * Function to return a field of type double.
     * @param fieldType: desired output field.
     * @return a vector of the desired field.
     */ 
    const std::vector <double>& getDoubleFieldData(const FieldType& fieldType) const;

    /**
     * Function to return psi field array.
     */ 
    const std::vector <double>& getPsiArray() const;

    /**
     * Function to return poloidal current field vector.
     */ 
    const std::vector <double>& getPoloidalCurrentArray() const;

    /**
     * Function to return r coordinate of limiter points.
     */ 
    const std::vector <double>& getLimiterPointsR() const;

    /**
     * Function to return z coordinate of limiter points.
     */ 
    const std::vector <double>& getLimiterPointsZ() const;

    /**
     * Function to return r coordinate of physical boundary.
     */ 
    const std::vector <double>& getBdryPointsR() const;

    /**
     * Function to return z coordinate of physical boundary. 
     */  
    const std::vector <double>& getBdryPointsZ() const;

    /**
     * Function to return a vector of size 4 for the rectangular domain box.
     * rMin = box[0], zMin = box[1], rMax = box[2], zMax = box[3]
     */ 
    const std::vector <double>& getDomainBox() const;

    /**
     * Function to return a vector of psi spline coefficients.
     */ 
    const std::vector <double>& getPsiSplineCoefficients() const;

    /**
     * Function to return a vector of size 3 for the shape of coefficients array.
     * The original array is 3D which is flattened in a vector.
     */ 
    const std::vector <int>& getPsiSplineShape() const;

    /**
     * Function to return a vector of poloidal current spline coefficients.
     */ 
    const std::vector <double>& getCurrentSplineCoefficients() const;

    /**
     * Function to return a vector of size 2 for the shape of coefficients array.
     * The original array is 2D which is flattened in a vector.
     */ 
    const std::vector <int>& getCurrentSplineShape() const;

    /**
     * Function to return vector of psi values at Chebyshev points on the grid cells.
     */ 
    const std::vector <double>& getPsiAtChebyshevPoints() const;

    /**
     * Function to return a vector of bicubic spline coefficients on the grid cells.
     */ 
    const std::vector <double>& getBiCubicSplineCoefficients() const;

  private:
    // Grid Data
    std::vector <double> rPoints;
    std::vector <double> zPoints;
    std::vector <double> psiField;

    // 1D Arrays
    std::vector <double> psiArray;
    std::vector <double> poloidalCurrentArray;
  
    // Physical coordinates (limiter and plasma boundary points)
    // Limiter is basically wall curve given in eqdsk file.
    // plasma boundary (bdry) is separatrix boundary given in
    // eqdsk (not the one we traced from our calculations)
    // NOTE: Need to discuss if XGC needs this info
    std::vector <double> rLimiterPoints;
    std::vector <double> zLimiterPoints;
    std::vector <double> rBdryPoints;
    std::vector <double> zBdryPoints;
    std::vector <double> domainBox;

    // Splines Coefficients
    std::vector <double> psiSplineCoefficients;
    std::vector <int> psiSplineShape;
    std::vector <double> currentSplineCoefficients;
    std::vector <int> currentSplineShape;

    // Psi Values for Chebyshev points
    std::vector <double> psiValueAtChebyshevPoints; 

   // Bicubic spline coefficients for psi grid
   std::vector <double> biCubicSplineCoefficients;
};

#endif
