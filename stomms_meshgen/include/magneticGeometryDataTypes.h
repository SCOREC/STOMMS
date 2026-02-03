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

/**
 * A class to define geometric model on a plane.
 */
class Plane{
  public:
    std::vector <Face> modelFaces;
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    Vertex oPoint;
    int planeNumber;  // plane number starting from 0 to numPlanes-1
};

/**
 * Struct VmecData contains all the input VmecData.
 */
struct VmecData{
  double majorR;  // Major radius of the reactor.
  double minorR;  // Minor radius of the reactor.
  int nSurf;  // Number of poloidal flux surfaces.
  int nMode;  // Number of modes for Fourier series.
  std::vector <double> R;  // Vector of cosines coeffiecents of R for Fourier series.
  std::vector <double> Z;  // Vector of sines coeffiecents of Z for Fourier series.
  std::vector <double> L;  // Vector of sines of lambdas coeffiecents for Fourier series.
  std::vector <double> iota;  // Vector of iota values corresponding to flux surfaces.
  std::vector <double> psi;  // Vector of list of psi values of flux surfaces.
  std::vector <double> xm;   // poloidal modes.
  std::vector <double> xn;   // Toroidal modes.
};

/**
 * Struct bmwData contains the data from BMW file.
 */
struct BmwData{
  // Populate it as we move forward.
};

/** 
 * Struct eqdskData contains the magnetic field information from eqdsk file.
 */
class EqdskData{
  public:
    /**
     * Eqdsk Constructor.
     */ 
    EqdskData(){};

    /**
     * Eqdsk Constructor.
     * @param input: class holding all the input data.
     * @param oPoint: primary oPoint.
     * @param xPoint: primary xPoint.
     */ 
    EqdskData(const Inputs& input, const PhysicsPoint& oPoint, const double& psiCoreBoundary);

    /**
     * Returns the values of psi at a physical location defined by pt.
     * @param pt: physical location on the domain defined by struct Point.
     * @return the psi value at the point.
     */ 
    double getPsiAtPoint(const Point& pt);

    /**
     * Returns the values of psi gradients at a physical location defined by pt.
     * @param pt: physical location on the domain defined by struct Point.
     * @return an array of psi gradients in each direction(dpsi/dr, dpsi/dz, dpsi/dphi) 
     */ 
    std::array<double,3> getPsiGradAtPoint(const Point& pt);

    /**
     * Returns the absolute value of psi gradients at a physical location defined by pt.
     * @param pt: physical location on the domain defined by struct Point.
     * @return absolute value of psi gradients. 
     */ 
    double getPsiGradAbsoluteAtPoint(const Point& pt);

    /**
     * Returns the psi partial derivative at a physical location defined by pt.
     * @param pt: physical location on the domain defined by struct Point.
     * @param dr: order of derivative along R-direction.
     * @param dz: order of derivative along Z-direction.
     * @return partial derivatives at the given point.
     */ 
    double getPsiDerivativeAtPoint(const Point& pt, int dr, int dz);

    /**
     * Returns the values of poloidal current at given psi value.
     * @param psi: psi value at which poloidal current is needed.
     * @return the poloidal current value.
     */ 
    double getCurrentAtPsi(double psi);

    /**
     * Function to convert normalized psi to psi for given eqdsk.
     * @param normPsi: normalized psi value to be converted to psi.
     * @return: psi value.
     */ 
    double convertNormToPsi(double normPsi);

    /**
     * Function to convert psi to normalized psi for given eqdsk.
     * @param psi: psi value to be converted to normalized psi.
     * @return: normalized psi value.
     */ 
    double convertPsiToNorm(double psi);
    
    /**
     * Function to update point pt, to the nearest point with goalPsi.
     * @param pt: point to readjust on goalPsi.
     * @param goalPsi: the desired psi value.
     * @return true if found a point with goalPsi and readjusted pt to it.
     */ 
    bool snapToPsi(Point& pt, double goalPsi);

    /**
     * Function to return magnetic field components at a physical location.
     * @param ptArray: array defining three coordinates of a physical point.
     * @param dpsi: a vector to contain components of magnetic field (B). Size 2 for 2D, and 3 for 3D.
     * @param dimension: 2 for 2D, 3 for 3D. Any other dimensions will throw an error.
     * returns 0 for success. 
     */ 
    int magneticField(std::array <double,3> ptArray, std::vector <double>& dpsi, int dimension);

    /**
     * Runge-Kutta method for numerical integration.
     * @param point0: physical coordinates of the given point.
     * @param point1: physical coordinates of the output  point.
     * @param dt: time step-size.
     * @param dimension: 2 for 2D, 3 for 3D. Any other dimensions will throw an error.
     * returns true = outside of the domain, false = inside of the domain (success). 
     */ 
    bool rk4(Point& point0, Point& point1, double dt, int dimension);

    /**
     * To find a next point by going around on edges of box boundary when tracing point hits a 
     * box boundary using Newton method.
     * @param[in,out] pt: a position to be adjusted.
     * @param psi: a psi value for the adjusted position to have.
     * @param side: an indicator of a side of a box boundary where tracing point hits.
     * @return true if the position with psi is found on an edge of the box
     */ 
    bool findNextPsiPointOnBoundary(Point& pt, double psi, int side);

    /**
     * Given a target psi and an initial guess, find a point in direction of dir.
     * @param targetPsi: psi value on which the point is desired.
     * @param startPoint: initial guess.
     * @param dir: direction array.
     * @param finalPoint: the point with target psi value on the vector dir.
     * @return 1 for success, 0 otherwise.
     */ 
    int findPsiPt(double targetPsi, Point startPoint, std::array<double,2> dir, Point& finalPoint);
    int findPsiPtOnLine(double targetPsi, const Point& pt1, const Point pt2, Point& finalPoint);
    /**
     * Given the psi value, this function finds the coordinates of the point on a 
     * horizontal line from axis to the box (either inward or outward).
     */ 
    Point convertPsiToPoint(double psi);

    /**
     * Function to get domain bounding box.
     */
    DomainBox getDomainBox(); 

    /**
     * Checks if a point pt is inside or outside of the bounding box.
     * @param pt: point to check.
     * @return true if point is inside, false for outside.
     */
    bool insideBox(const Point& pt);
    
    /**
     * Checks if a point pt is inside or outside of the bounding box.
     * @param pt: point to check.
     * @return true if point is inside, false for outside.
     */
    bool insideBox(const std::array <double,3>& pt);

    /**
     * Returns the number of poloidal planes (user input).
     */
    const int getNumPlanes() const;

    /**
     * Returns step size in radian (user input).
     */
    const double getStepRadians() const;

    /**
     * Returns psi tolerance for calcuations (user input).
     */
    const double getPsiTolerance() const;

    /**
     * Returns spacing tolerance (user input).
     */
    const double getSpacingToleranceOptimal() const;

    /**
     * Returns struct FluxData containing input data of flux.
     */
    const FluxData& getFluxInputData() const;

    /**
     * Function to check if start of the flux curve is random or not.
     */
    const bool& randomStart() const;
 
    /**
     *  Returns the type of desired intra curve spacing option.
     */  
    const int& getIntraCurveSpacingOption() const;

    /**
     * Returns the absolute spacing tolerance
     */ 
    const double& getSpacingToleranceAbsolute() const;

    /*
     * Function to check if the small spacing variation is allowed or not.
     */ 
    const bool& getIntraCurveSpacingSmallVariation() const;
  private:
  // input data.
  // Write a function to read these values directly from inputs (LATER).
  bool reversePsi = false;
  bool inboardStart = false;
  bool fluxRandomStart = true;
  int numPlanes = 128;
  bool stepRadians = 0.00125;
  double psiTolerance = 1e-8;
  double spacingToleranceOptimal = 0.5;
  double spacingToleranceAbsolute = 0.61803398874989484820;
  bool intraCurveSpacingSmallVariation = false;
  bool zeroXptWall = false; 
  std::array <double, 4> boundingBox;  // bounding box
  FluxData fluxInputData;
  int intraCurveSpacingOption = -1;

  // Derived data.
  PhysicsPoint axis;
  double psiCoreEdge;  // last curve of core
};

#endif
