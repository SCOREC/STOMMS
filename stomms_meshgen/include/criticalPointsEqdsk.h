#ifndef CRITICALPOINTSEQDSK_H
#define CRITICALPOINTSEQDSK_H

#include "criticalPoints.h"
#include "gfileUtil.h"

/* 
 * Struct Simplex contains the 3 points for 2D simplex.
 * Can be extended. Num of points = dimension + 1.
*/
struct Simplex{
  Simplex(){};
  Simplex(std::vector <Point> pts);
  Simplex(Point pt1, Point pt2, Point pt3);  
  Point point1;
  Point point2;
  Point point3;

  // Accessing simplex in array form is desireable 
  // at some points in code.
  std::array<std::array<double, 2>, 3> points;

  // Extend it to 3D if needed in future.
}; 

/*
 * Struct SimplexGrid takes in grid resolutions and creates a vector
 * of Simplexes.
 */
struct SimplexGrid{
  SimplexGrid(){};

  /*
   * int xResolution (in): resolution along dimension 1 (x, or r).
   * int yResolution (in): resolution along dimension 2 (y, or z).
   * const std::array<double,4>& box (in): domain box. 
   */ 
  SimplexGrid(int xResolution, int yResolution, const std::array<double,4>& box);
  std::vector <Simplex> simplexVec;  // vector of resulting simplexes.
};

/*
 * Class SimplexMethod 
 * 2D Downhill Simplex Method for finding  critical points
 * Sources modified from Numerical Recipe Ch. 10.4.
 */
class SimplexMethod {
  public:
    SimplexMethod(){};

    /*
     * SimplexMethod takes a SimplexGrid, and a bounding box, and calculates 
     * the minimums in the domain box. This one is specific to the gradient
     * of the psi field (grad_psi).
     * const SimplexGrid& simplexGrid (in): a grid of 2D simplexes.
     * const std::array<double,4>& box (in): domain box.
     */ 
    SimplexMethod(const SimplexGrid& simplexGrid, const std::array <double,4>& box, const bool& useReversePsi);

    /*
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

    /*
     * A function that extrapolates by a factor (factor) through the face of the simplex across 
     * from the high point,tries it, and replaces the high point if the new point is better.
     * See function amotry in Numerical Recipe Ch. 10.4. 
     * std::array<std::array<double, 2>, 3>& points (in, out) : three points of the Simplex.
     * std::array<double,3>& y (in): Field values at three points (grad_psi).
     * std::array<double,2>& pSum (in): Sum of individual coordinates. 
     * int ihi (in): point with highest field value in the domain.
     * double factor (in): a factor to extrapolate for different types of simplex operations.
     * returns a trial field value (yTry).
     */ 
    double evaluateTrialPoint(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, 
                              std::array<double,2>& pSum, int ihi, double factor);

    /*
     * a function to return estimated minimum at y[0].
     * See function amoeba in Numerical Recipe Ch. 10.4.
     * std::array<std::array<double, 2>, 3>& points (in, out) : three points of the Simplex.
     * std::array<double,3>& y (in, out): Field values at three points (grad_psi).
     * const double& fToleranceAbs (in): absolute function tolerance.
     * const double& fToleranceRel (in): relative function tolerance.
     * int& nFunc (in): an iterator for the number of times a field value is calcuated.
     * returns 0 if no minium is found, else 1.
     */   
    int evaluateMinimum(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, 
                        const double& fToleranceAbs, const double& fToleranceRel, int& nFunc);
};

/*
 * Class to evaluate critical points from EQDSK file using gradient of psi field.
 * grad_psi = 0. Filters out any points that are outside the reactor wallcurve.
 */
class CriticalPointsEqdsk{
  public:
    CriticalPointsEqdsk(){};

    /*
     * Given wall curve, (already have access to eqdsk file and domain box) using gfileUtil.h function),
     * evaluates the critical points in the doamin.
     * const WallCurve& wall (in): input wall curve.
     */ 
    CriticalPointsEqdsk(const WallCurve& wall, const bool& reversePsi);

    /*
     * Function to return O-points in the domain.
     */ 
    const std::vector <PhysicsPoint>& getOPoints();

    /*
     * Function to return X-points in the domain.
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

    /*
     * Function to call Simplex method, and to return a vector of minimums from Simplex method.
     */ 
    std::vector <Point> findMinimumSimplexMethod();

    /*
     * Function to find a minimum given an initial guess (these guesses are calculated from Simplex method).
     * const Point& initialGuess (in): given initial guess.
     * Point& finalPosition (out): final position of minimum after running Netwon method.
     * const std::array<double,4> domain (in): domain box. Could have accessed it directly from class data member 
     * but tried to give it general in case we need it in future.
     * returns 0 if minium is found (success).
     */ 
    int findMinimumNewtonMethod(const Point& initialGuess, Point& finalPosition, const std::array<double,4> domain); 
};

// Helper Functions:

/*
 * Given a point, test if the point is saddle point, minimum, or maximum and return point type
 * based on that. 
 * Second Partial Derivative Test: https://en.wikipedia.org/wiki/Second_partial_derivative_test
 * const Point& pt (in): point to be tested.
 * returns point type (PointType::XPoint, PointType::OPoint, PointType::None)
 */
PointType getPointType(const Point& pt, bool reversePsi);

#endif
