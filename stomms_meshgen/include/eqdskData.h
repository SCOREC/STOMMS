#ifndef EQDSKDATA_H
#define EQDSKDATA_H

#include "input.h"
#include "criticalPoints.h"
#include "modelMetaData.h"

/** 
 * Class eqdskData contains the magnetic field information from eqdsk file.
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
     * @param planeData: planer meta data
     * @param oPoint: primary oPoint.
     * @param psiCoreBoundary: psi value of last closed flux curve or innermost separatrix (primary)
     */ 
    EqdskData(const Inputs& input, const PlaneMetaData& planeData, const PhysicsPoint& oPoint, const double& psiCoreBoundary);

    /**
     * Set the intra curve gradient spacing.
     */
    void setintraCurveSpacingGradPsi();    
 
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
     * Function to get spacing between the two psi values bounding the psiNorm.
     * @param psiNorm: normalized psi value.
     * @return spacing between two psi values bounding psiNorm.
     */
    double getInterCurveSpacingLinear(double psiNorm);

    /**
     * Function to get intra curve spacing for the given psi and a starting point.
     * @pt: A starting point (needed in non-field following case). TO-DO: Make it optional in future.
     * @param psiNorm: normalized psi value.
     * @return returns desired node spacing.
     */ 
    double getNodeSpacing(const Point& pt, double psiNorm);
 
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
     * Function to set eqdsk parameters from inputs.
     */ 
    void setParameters(const Inputs& in);

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

    /*
     * Function to get minimum spacing between last two points of a flux curve.
     */ 
    const double& getIntraCurveMinLengthLastEdge() const;
  private:
  // input data.
  bool reversePsi;
  bool inboardStart;
  bool fluxRandomStart;
  int numPlanes;
  double stepRadians;
  double psiTolerance;
  double spacingToleranceOptimal;
  double spacingToleranceAbsolute;
  bool intraCurveSpacingSmallVariation;
  bool zeroXptWall; 
  std::array <double, 4> boundingBox;  // bounding box
  FluxData fluxInputData;
  int intraCurveSpacingOption;
  double intraCurveMinLengthLastEdge; 
  double intraCurveSpacingPropFacMax;  // internal use for class
  double intraCurveSpacingPropFacMin;  // internal use for class 

  // Derived data.
  PhysicsPoint axis;
  double psiCoreEdge;  // last curve of core
  PlaneMetaData planeMetaData;
  std::vector <double> fluxValues;
  std::vector <double> intraCurveSpacingGradPsi;
};

#endif
