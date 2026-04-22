#ifndef CRITICALPOINTSEQDSK_H
#define CRITICALPOINTSEQDSK_H

#include "criticalPoints.h"
#include "gfileUtil.h"

/** 
 * Struct Simplex contains the 3 points for 2D simplex.
 * Can be extended. Num of points = dimension + 1.
*/
struct Simplex{
  Simplex(){};

  /**
   * Contructor
   * @param pts: a vector of points defining the simplex. Currently needs three points.
   */ 
  Simplex(std::vector <Point> pts);

  /**
   * Constructor
   * @param pt1: point one of the simplex.
   * @param pt2: point two of the simplex.
   * @param pt3: point three of the simplex.
   */ 
  Simplex(Point pt1, Point pt2, Point pt3);

  // Points of Simplex  
  Point point1;
  Point point2;
  Point point3;

  // Accessing simplex in array form is desireable 
  // at some points in code.
  std::array<std::array<double, 2>, 3> points;

  // Extend it to 3D if needed in future.
}; 

/**
 * Struct SimplexGrid takes in grid resolutions and domain box to creates a vector
 * of Simplexes.
 */
struct SimplexGrid{
  SimplexGrid(){};

  /**
   * Constructor to create a vector of simplexes given the resolution and domain box.  
   * @param xResolution: resolution along dimension 1 (x, or r).
   * @param yResolution: resolution along dimension 2 (y, or z).
   * @param box: an array of size 4 defining domain box [format of array: xmin, ymin, xmax, ymax]. 
   */ 
  SimplexGrid(int xResolution, int yResolution, const std::array<double,4>& box);
  std::vector <Simplex> simplexVec;  // vector of resulting simplexes.
};

/**
 * Class SimplexMethod 
 * 2D Downhill Simplex Method for finding  critical points
 * Sources modified from Numerical Recipe Ch. 10.4.
 */
class SimplexMethod {
  public:
    SimplexMethod(){};

    /**
     * Contructor: SimplexMethod takes a SimplexGrid, and a bounding box, and calculates the minimums 
     * in the domain box. This one is specific to the gradient of the psi field (grad_psi).
     * @param simplexGrid: a grid of 2D simplexes.
     * @param box: an array of size 4 defining domain box [format of array: xmin, ymin, xmax, ymax].
     * @param useReversePsi: if true, multiply psi values to -1. Else, use the psi values as it is.
     */ 
    SimplexMethod(const SimplexGrid& simplexGrid, const std::array <double,4>& box, const bool& useReversePsi);

    /**
     * Function to return the minimum found from Simplex method. 
     */  
    const std::vector <Point>& getCandidates();
  private:
    // Input information
    std::array <double, 4> domainBox;  // domain box
    SimplexGrid grid;  // simplex grid.
    bool reversePsi = false; 

    // Output information    
    std::vector <Point> candidates;  

    // Member functions:

    /**
     * A function that extrapolates by a factor (factor) through the face of the simplex across 
     * from the high point, tries it, and replaces the high point if the new point is better.
     * See function amotry in Numerical Recipe Ch. 10.4. 
     * @param points: an array of three points of the Simplex.
     * @param y: an array of field values at three points (field = grad_psi).
     * @param pSum: a 2D array to hold sum of individual coordinates. 
     * @param ihi: point with highest field value in the domain.
     * @param factor: a factor to extrapolate for different types of simplex operations.
     * @return a trial field value (yTry).
     */ 
    double evaluateTrialPoint(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, 
                              std::array<double,2>& pSum, int ihi, double factor);

    /**
     * A function to return estimated minimum at y[0].
     * See function amoeba in Numerical Recipe Ch. 10.4.
     * @param points: an array of three points of the Simplex.
     * @param y: an array of field values at three points (field = grad_psi).
     * @param fToleranceAbs: absolute function tolerance.
     * @param fToleranceRel: relative function tolerance.
     * @param nFunc: an iterator for the number of times a field value is calcuated.
     * @return 0 if no minium is found, else 1.
     */   
    int evaluateMinimum(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, 
                        const double& fToleranceAbs, const double& fToleranceRel, int& nFunc);
};

/**
 * Class to evaluate critical points from EQDSK file using gradient of psi field.
 * grad_psi = 0. Filters out any points that are outside the reactor wallcurve.
 */
class CriticalPointsEqdsk{
  public:
    CriticalPointsEqdsk(){};

    /**
     * Constructor: Given wall curve, (already have access to eqdsk file and domain box 
     * using gfileUtil.h function), evaluates the critical points in the doamin.
     * @param wall: input wall curve.
     * @param reversePsi: if true, multiply psi values to -1. Else, use the psi values as it is.
     */ 
    CriticalPointsEqdsk(const WallCurve& wall, const bool& reversePsi);

    /**
     * Function to a vector return O-points in the domain.
     */ 
    const std::vector <PhysicsPoint>& getOPoints();

    /**
     * Function to return a vector X-points in the domain.
     */ 
    const std::vector <PhysicsPoint>& getXPoints();
  private:
    // Input information
    std::array <double, 4> domainBox;
    WallCurve wallCurve;
    std::vector <Point> wallPoints;
    bool useReversePsi;

    // Information to evaluate.
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;

    /**
     * Function to call Simplex method, and to return a vector of minimums from Simplex method.
     */ 
    std::vector <Point> findMinimumSimplexMethod();

    /**
     * Function to find a minimum given an initial guess (these guesses are calculated from Simplex method).
     * @param initialGuess: initial guess point. 
     * @param finalPosition: final position of minimum after running Netwon method.
     * @param domain: an array of size 4 defining domain box [format of array: xmin, ymin, xmax, ymax]. 
     * @return 0 if minium is found (success).
     */ 
    int findMinimumNewtonMethod(const Point& initialGuess, Point& finalPosition, const std::array<double,4> domain); 
};

// Helper Functions:

/**
 * Given a point, test if the point is saddle point, minimum, or maximum and return point type
 * based on that. Uses determinant of Hessian matrix. 
 * Second Partial Derivative Test: https://en.wikipedia.org/wiki/Second_partial_derivative_test
 * @param pt: point to be tested.
 * @param reversePsi: if true, multiply psi values to -1. Else, use the psi values as it is.
 * @return point type (PhysicsPointType::XPoint, PhysicsPointType::OPoint, PhysicsPointType::None)
 */
PhysicsPointType getPointType(const Point& pt, bool reversePsi);

#endif
