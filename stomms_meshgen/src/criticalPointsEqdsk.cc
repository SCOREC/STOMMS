#include "criticalPointsEqdsk.h"

/***********************************************/
// Structs for Simplex Method
// 1- Simplex 
// 2- Simplex Grid
/***********************************************/
// Simplex
Simplex::Simplex(Point pt1, Point pt2, Point pt3):point1(pt1), point2(pt2), point3(pt3)
{
  points = {{ {point1.x, point1.y}, {point2.x, point2.y}, {point3.x, point3.y} }};
}

Simplex::Simplex(std::vector <Point> pts)
{
  assert (pts.size() == 3 || pts.size() == 4); // 3 for 2D, 4 for 3D
  point1 = pts[0];
  point2 = pts[1];
  point3 = pts[2];

  points = {{ {point1.x, point1.y}, {point2.x, point2.y}, {point3.x, point3.y} }};
}

// Simplex Grid
SimplexGrid::SimplexGrid(int xResolution, int yResolution, const std::array<double,4>& box)
{
  double xMin = box[0], yMin = box[1];
  double xMax = box[2], yMax = box[3];

  for (int i = 0; i < xResolution; i++)
  {
    for (int j = 0; j < yResolution; j++)
    {
      Point p1, p2, p3;
      p1.z = p2.z = p3.z = 0.0;

      // Point 1
      p1.x = (xMin*(xResolution-i) + xMax*i)/double(xResolution);
      p1.y = (yMin*(yResolution-j) + yMax*j)/double(yResolution);

      // Point 2
      p2.x = (xMin*(xResolution-i-1) + xMax*(i+1))/double(xResolution);
      p2.y = p1.y;

      // Point 3
      p3.x = p1.x;
      p3.y = (yMin*(yResolution-j-1) + yMax*(j+1))/double(yResolution);

      // To make sure we are not getting a point just outside of the
      // computational domain because of numerical inconsistencies.
      checkBounds(p1, box);
      checkBounds(p2, box);
      checkBounds(p3, box);

      // Create Simplex
      Simplex simplex(p1,p2,p3);
      simplexVec.push_back(simplex);
    }
  }
  assert (simplexVec.size() == (xResolution*yResolution));
}

/***********************************************/
// Class SimplexMethod
/***********************************************/
SimplexMethod::SimplexMethod(const SimplexGrid& simplexGrid, const std::array <double,4>& box, const bool& useReversePsi)
{
  // Step 1: Read domain information and grid of simplexes.
  domainBox = box;
  grid = simplexGrid;
  reversePsi = useReversePsi;

   // Step 2: Set up variables for Simplex points, tolerance, and number of iterations.
  std::array<std::array<double, 2>, 3> simplexPoints;
  double absoluteTolerance = 1E-8;
  double relativeTolerance = 1E-8;
  int nFunc;

  // Now variables for field values (psi values and its gradients)
  std::array<double, 3> gradPsi;
  double psi;
  int ierr;

  
  // Step 3: Call the simplex method for individual simplexes.
  // Iterate over each simplex.
  int numSimplex = grid.simplexVec.size();
  for (int i = 0; i < numSimplex; i++)
  {
    Simplex simplex = grid.simplexVec[i];
    simplexPoints = simplex.points;
    
    // Step 3.1: Get field value (grad_psi) for each of the points of simplex.
    for (int j = 0; j < 3; j++)
      eval_field_grad_abs2(&simplexPoints[j][0], &simplexPoints[j][1], &gradPsi[j], &ierr, reversePsi);

    // Step 3.2: Evaluate minimums from each simplex. If returned 0, means no minium found, so move
    // to next simplex. If returns 1, means a minium is found from given simplex.
    nFunc = 0;    
    if (evaluateMinimum(simplexPoints, gradPsi, absoluteTolerance, relativeTolerance, nFunc) == 0)
      continue;

    // Step 3.3: Save the point in the vector.
    Point possibleMinimum(simplexPoints[0][0], simplexPoints[0][1]);
    candidates.push_back(possibleMinimum);
  } 
}

// A function to find point with highest value. If the given point ihi isnt the higest, replace the point by
// making a new simplex based on the factor.(factor = 1 is reflection, = 0.5 is contration, = 2 is expansion).
double SimplexMethod::evaluateTrialPoint(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, 
                                         std::array<double,2>& pSum, int ihi, double factor)
{
  // Step 1: Setup the variables.
  double factor1, factor2, yTry;
  std::array<double,2> pTry;

  // Step 2: Based on given factor, setup the two factors needed to perform one
  // of three operations.
  factor1 = (1.0 - factor)*0.5;
  factor2 = factor1 - factor;

  // Step 3: Evalaute the trial point.
  for (int i = 0; i < 2; i++)
    pTry[i] = pSum[i]*factor1 - points[ihi][i]*factor2;

  // Step 4: Readjust the point if they are out of the domain.  
  pTry[0] = std::min(std::max(pTry[0], domainBox[0]), domainBox[2]);
  pTry[1] = std::min(std::max(pTry[1], domainBox[1]), domainBox[3]);

  //  Step 5: Eva;uate field value at trial point. If its better than
  //  the highest point (ihi) replace ihi with trial point. Update 
  //  pSum and return trial point.
  int ierr;
  eval_field_grad_abs2(&pTry[0], &pTry[1], &yTry, &ierr, reversePsi);
  if (yTry < y[ihi]) 
  {
    y[ihi] = yTry;
    for (int i = 0; i < 2; i++)
    {
      pSum[i] += pTry[i] - points[ihi][i];
      points[ihi][i] = pTry[i];
    }
  }

  return yTry;    
}

// A function to return estimated minimum at y[0].
int SimplexMethod::evaluateMinimum(std::array<std::array<double, 2>, 3>& points, std::array<double,3>& y, 
                                   const double& fToleranceAbs, const double& fToleranceRel, int& nFunc)
{
  // Step 1: Define the variables needed 
  // ihi = index of highest field value, ilo = lowest, inhi = next highest (intermediate)
  int ihi, ilo, inhi, ierr; 
  double rTolerance, aTolerance, sum, ySave, yTry;
  std::array <double, 2> pSum;
  int nMax = 5000;  // maximum iteration
  double psiToleranceAbsolute = 1e-10;
  nFunc = 0;

  // Step 2: Iterate over number of dimension and get the sum of coordinates
  // of individual dimension (sum over x and sum over y)
  for (int j = 0; j < 2; j++)  // dimension
  {
    pSum[j] = 0.0;
    sum = 0.0;
    for (int i = 0; i < 3; i++)  // Number of points in simplex
      sum += points[i][j];
    pSum[j] = sum;
  }

  // Step 3: Keep running until an exit condition is hit (either found the minium or exceeded
  // the maximum allowed iterations).
  while (true)
  {
    // Step 3.1: Determine the highest, lowest, and next highest points in the simplex
    // based on their field value.
    ilo = 0;
    if (y[0] > y[1])
      ihi = 0, inhi = 1;
    else
      ihi = 1, inhi = 0;

    for (int i = 0; i < 3; i++)
    {
      if (y[i] <= y[ilo])
        ilo = i;
      if (y[i] > y[ihi])
      {
        inhi = ihi;
        ihi = i;
      }
      else if (y[i] > y[inhi] && i != ihi)
        inhi = i;
    }

    // Step 3.2: If minium is found exit the loop
    rTolerance = 2.0 * fabs(y[ihi] - y[ilo])/(fabs(y[ihi]) + fabs(y[ilo]) + psiToleranceAbsolute); // psiToleranceAbsolute to prevent from division by 0
    aTolerance = fabs(y[ihi] - y[ilo]);
    if(rTolerance < fToleranceRel && aTolerance < fToleranceAbs) 
    {
      std::swap(y[0], y[ilo]);
      for(int i = 0; i < 2; i++) 
        std::swap(points[0][i], points[ilo][i]);
      break;
    }

    // Step 3.3: If number of iterations exceeded maximum iterations, exit the function.
    if(nFunc >= nMax) 
    {
      std::cout << "Downhill Simplex Method exceeds 5000 evaluations of magnetic field values for the Simplex \n";;
      std::cout <<  points[0][0] << "," << points[0][1] << "|" << points[1][0] << "," << points[1][1]<< "|" << points[2][0] << "," << points[2][1] << "\n";
      exit(1);
    }

    nFunc += 2;

    // Step 3.3: Begin a new iteration. First extrapolate by a factor -1 through the face of the simplex 
    // across from the high point, i.e., reflect the simplex from the high point.
    yTry = evaluateTrialPoint(points, y, pSum, ihi, -1.0);

    if(yTry <= y[ilo])
      //Gives a result better than the best point, so try an additional extrapolation by a factor 2.
      yTry = evaluateTrialPoint(points, y, pSum, ihi, 2.0);
    else if (yTry >= y[inhi]) 
    {
      //The reflected point is worse than the second-highest, so look for an intermediate lower point,
      // i.e., do a one-dimensional contraction.
      ySave = y[ihi];
      yTry = evaluateTrialPoint(points, y, pSum, ihi, 0.5);

      if(yTry >= ySave) 
      { //Can't seem to get rid of that high point. Better contract around the lowest (best) point.
        for (int i = 0; i < 3; i++) 
        {
          if (i != ilo) 
          {
            for(int j = 0; j < 2; j++)
              points[i][j] = pSum[j] = 0.5 * (points[i][j] + points[ilo][j]);
            eval_field_grad_abs2(&pSum[0], &pSum[1], &y[i], &ierr, reversePsi);
          }
        }

        nFunc += 2;

        for(int j = 0; j < 2; j++) 
        {
          sum = 0.0;
          for(int i = 0; i < 3; i++) 
            sum += points[i][j];
          pSum[j]=sum;
        }
      } 
    } 
    else 
      --nFunc;

    // Step 3.4: Check if all the points of the simplex are inside the domain or not.
    Point p1, p2, p3;
    p1.x = points[0][0], p1.y = points[0][1];
    p2.x = points[1][0], p2.y = points[1][1];
    p3.x = points[2][0], p3.y = points[2][1];
    if (!inDomain(p1, domainBox) || !inDomain(p2, domainBox) || !inDomain(p3, domainBox))
      return 0;
  }
  return 1;  //Sucssess
}

// Function to return the minimums from the simplex method. They 
// are called candidates for now because we will still run Newton
// Method.
const std::vector <Point>& SimplexMethod::getCandidates()
{
  return candidates;
}

/***********************************************/
// Class CriticalPointsEqdsk
/***********************************************/
CriticalPointsEqdsk::CriticalPointsEqdsk(const WallCurve& wall, const bool& reversePsi):
                                         wallCurve(wall), useReversePsi(reversePsi)
{
  std::cout << "========== CRITICAL POINTS SEARCH ==========\n";
  wallPoints = wallCurve.getPoints();

  // Step 1: Execute Simplex Method
  std::cout << "Simplex Method Started ..........\n";
  std::vector <Point> candidates = findMinimumSimplexMethod();

  // Step 2: Filter out the points that are outside the wall curve 
  // and only keep unique points (get rid of duplicates). Save 
  // unique points in a new vector.
  filterOutsideTheWallPoints(candidates, wallPoints);
  std::vector <Point> filtered = filterUniquePoints(candidates);  

  // Step 3: Execute Newton Method.
  std::vector <Point> updatedPoints;
  for (int i = 0; i < filtered.size(); i++)
  {
    Point pt = filtered[i], ptFinal;
    if (i == 0)
      std::cout << "Newton Method Started ..........\n";
 
    // Step 3.1: If point is outside wall curve, ignore it.
    if (windingNumberPolygonTest(pt, wallPoints) == 0)
      continue;

    // Step 3.4: If minimum, save it to the a new vector.
    int returnIndx = findMinimumNewtonMethod(pt, ptFinal, domainBox);
    if (returnIndx == 0)
      updatedPoints.push_back(ptFinal);
  } 

  // Step 4: Clear the old filtered vector and do filteration step again.
  filtered.clear();
  filterOutsideTheWallPoints(updatedPoints, wallPoints);
  filtered = filterUniquePoints(updatedPoints);
  updatedPoints.clear();

  // Step 5: Run the Hessian test to get point type.
  for (int i = 0; i < filtered.size(); i++)
  {
    Point pt = filtered[i];
    PointType ptType = getPointType(pt, useReversePsi);
    double psi;
    int ierr;
    eval_field_val(&pt.x, &pt.y, &psi, &ierr, useReversePsi);

    // Step 5.1: Setup PhysicsPoint.
    PhysicsPoint physicsPt(pt, psi, ptType);

    // Step 5.2: Save to respective x-point, opoint container.
    if (ptType == PointType::OPoint)
      oPoints.push_back(physicsPt);
    else if (ptType == PointType::XPoint)
      xPoints.push_back(physicsPt);
  }
}

// Find the critical points from EQDSK file.
std::vector <Point> CriticalPointsEqdsk::findMinimumSimplexMethod()
{
  // Step 1: Domain box bounds
  double bbox[4]; // min r, min z, max r, max z
  get_b_box_(bbox);
  domainBox = {bbox[0], bbox[1], bbox[2], bbox[3]};

  // Step 2: Set up  Simplex Method
  int x = 30;  // resolution on x axis
  int y = 30;  // resolution on y axis
  SimplexGrid simplexGrid(x, y, domainBox);

  // Step 3: Run the simplex method, and get miniums (candidates) from it.
  SimplexMethod simplexMethod(simplexGrid, domainBox, useReversePsi);
  std::vector <Point> minimumPoints = simplexMethod.getCandidates();

  return minimumPoints; 
}

int CriticalPointsEqdsk::findMinimumNewtonMethod(const Point& initialGuess, Point& finalPosition, const std::array<double,4> domain) 
{
  // Step 1: Check validity of the initial point.
  if(!inDomain(initialGuess, domain))
    return -1;
 
  // Step 2: Set up variables, and trial point.
  int iStep=0;
  int maxSteps=1000;
  std::array<double, 2> trialPosition;
  trialPosition[0] = initialGuess.x;
  trialPosition[1] = initialGuess.y;

  // Step 3: Find find value and derivates at the trial point.
  // Field value
  double psiVal;
  int ierr; 
  eval_field_val(&trialPosition[0], &trialPosition[1], &psiVal, &ierr, useReversePsi);

  // Field derivative
  std::array <double,3> dy;
  eval_field_grad(&trialPosition[0], &trialPosition[1], &dy[0], &ierr, useReversePsi);
  
  // Step 4: Find Second order derivatives
  // d^2 psi/dr^2, d^2 psi/drdz, d^2 psi/dz^2
  std::array <double,3> dy2;
  int ndr, ndz;
  double dr, dz, dl;
  ndr = 2; ndz = 0; eval_field_deriv(&trialPosition[0], &trialPosition[1], &ndr, &ndz, &dy2[0], &ierr, useReversePsi); if(ierr) return false;
  ndr = 1; ndz = 1; eval_field_deriv(&trialPosition[0], &trialPosition[1], &ndr, &ndz, &dy2[1], &ierr, useReversePsi); if(ierr) return false;
  ndr = 0; ndz = 2; eval_field_deriv(&trialPosition[0], &trialPosition[1], &ndr, &ndz, &dy2[2], &ierr, useReversePsi); if(ierr) return false;

  // Step 5: Newton Step. dr, dz limited by given maximal length
  double updateFactor = 1.0;
  double divisor = dy2[0]*dy2[2] - dy2[1]*dy2[1];  // (d^2 psi /dr^2) * (d^2 psi / dz^2) - (d^2 psi / drdz)^2
  dr = -updateFactor * (dy2[2]*dy[0] - dy2[1]*dy[1])/divisor; 
  dz = -updateFactor * (dy2[0]*dy[1] - dy2[1]*dy[0])/divisor;
  dl = sqrt(dr*dr+dz*dz);

  if(isinf(1./dl)) 
  {
    //when trial position is exactly on the point
    finalPosition.x = trialPosition[0]; 
    finalPosition.y = trialPosition[1];
    return 0;   // found a minimum. normal exit
  }

  // Step 6: Normalize and reconstruct dr, dz and update the position.
  double drUnit = dr/dl; 
  double dzUnit = dz/dl;
  double steplimit_rel = 0.1;  // 10% size of smaller box side limits one step
  double stepMax = steplimit_rel* std::min(domainBox[3] - domainBox[1], domainBox[2] - domainBox[0]);
  dl = std::min(dl, stepMax);
  dr = drUnit*dl; 
  dz = dzUnit*dl;

  //position update
  trialPosition[0] += dr; 
  trialPosition[1] += dz;
  iStep++;

  //check validity
  Point ptToCheck(trialPosition[0], trialPosition[1]);
  if(!inDomain(ptToCheck, domain))
    return -2;

  //psi(x_1)
  double psiValNext;
  eval_field_val(&trialPosition[0], &trialPosition[1], &psiValNext, &ierr, useReversePsi);

  //for exit quantities
  double psiDiffAbs = fabs(psiValNext - psiVal);
  double psiDiffRel = psiDiffAbs/psiVal;

  // Step 7: Iterate until get the minium or reach exit conditions.
  double tolerance = 1e-8;
  while(!(psiDiffAbs < tolerance || psiDiffRel < tolerance) && iStep < maxSteps) 
  {

    // psi(x_n)
    psiVal = psiValNext;

    // dpsi/dr, dpsi/dz
    eval_field_grad(&trialPosition[0], &trialPosition[1], &dy[0], &ierr, useReversePsi);

    // d^2 psi/dr^2, d^2 psi/drdz, d^2 psi/dz^2
    ndr = 2; ndz = 0; eval_field_deriv(&trialPosition[0], &trialPosition[1], &ndr, &ndz, &dy2[0], &ierr, useReversePsi); if(ierr) return false;
    ndr = 1; ndz = 1; eval_field_deriv(&trialPosition[0], &trialPosition[1], &ndr, &ndz, &dy2[1], &ierr, useReversePsi); if(ierr) return false;
    ndr = 0; ndz = 2; eval_field_deriv(&trialPosition[0], &trialPosition[1], &ndr, &ndz, &dy2[2], &ierr, useReversePsi); if(ierr) return false;

    //dr, dz limited by given maximal length
    divisor = dy2[0]*dy2[2] - dy2[1]*dy2[1];  // (d^2 psi /dr^2) * (d^2 psi / dz^2) - (d^2 psi / drdz)^2
    dr = -updateFactor*(dy2[2]*dy[0] - dy2[1]*dy[1])/divisor; 
    dz = -updateFactor*(dy2[0]*dy[1] - dy2[1]*dy[0])/divisor;
    dl = sqrt(dr*dr+dz*dz);

    if(isinf(1./dl)) break; //when pos_trial is exactly on the point

    drUnit = dr/dl; 
    dzUnit = dz/dl;
    dl = std::min(dl, stepMax);
    dr = drUnit*dl; 
    dz = dzUnit*dl;

    //position update
    trialPosition[0] += dr; 
    trialPosition[1] += dz;
    iStep++;

    //check validity
    Point ptToCheck(trialPosition[0], trialPosition[1]);
    if(!inDomain(ptToCheck, domain))
      return -1;

    //psi(x_n+1)
    eval_field_val(&trialPosition[0], &trialPosition[1], &psiValNext, &ierr, useReversePsi);

    //for exit quantities
    psiDiffAbs = fabs(psiValNext - psiVal);
    psiDiffRel = psiDiffAbs/psiVal;
  }
  if(iStep >= maxSteps)
    return -3;
  else 
  {
    finalPosition.x = trialPosition[0]; 
    finalPosition.y = trialPosition[1];
    return 0;   // found a minimum. normal exit
  }
}

// Function to return O-points in the domain.
const std::vector <PhysicsPoint>& CriticalPointsEqdsk::getOPoints()
{
  return oPoints;
}

// Function to return X-points in the domain.
const std::vector <PhysicsPoint>& CriticalPointsEqdsk::getXPoints()
{
  return xPoints;
}

/***********************************************/
// Helper Functions
/***********************************************/
PointType getPointType(const Point& pt, bool reversePsi)
{
  int ier;
  double d2[3];

  // Step 1: Evaluate d^2(psi)/dr^2
  int dr = 2, dz = 0;
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &d2[0], &ier, reversePsi);
  assert(ier == 0);

  // Step 2: Evaluate dpsi/drdz
  dr = dz = 1;
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &d2[1], &ier, reversePsi);
  assert(ier == 0);

  // Step 3: Evaluate d^2(psi)/dz^2
  dr = 0, dz = 2;
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &d2[2], &ier, reversePsi);
  assert(ier == 0);

  // Step 4: Second Partial Derivative Test
  // https://en.wikipedia.org/wiki/Second_partial_derivative_test
  double det = d2[0]*d2[2] - d2[1]*d2[1];
  assert(det != 0);

  // Step 5: Evaluate point type.
  PointType pointType;
  if (det < 0)
    pointType = PointType::XPoint;     // Saddle point
  else if (d2[0] > 0)
    pointType = PointType::OPoint;     // Minimum
  else if (d2[0] < 0)
    pointType = PointType::None;     // Maximum
  
  return pointType;
}

