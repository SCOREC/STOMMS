#include "magneticGeometryDataTypes.h"

/***********************************************/
// Class: EqdskData
/***********************************************/
EqdskData::EqdskData(const Inputs& input, const PhysicsPoint& oPoint, const double& psiCoreBoundary)
{
  // Step 1: Read properties of eqdsk file from the inputs
  reversePsi = input.useReversePsi();
  numPlanes = input.getNumTokamakPlanes();
  InputData inputData = input.getInputData();
  fluxInputData = inputData.fd; 

  // Step 2: Domain box bounds
  double bbox[4]; // min r, min z, max r, max z
  get_b_box_(bbox);
  boundingBox = {bbox[0], bbox[1], bbox[2], bbox[3]};

  // Step 3: Domain definition needs primary o and x points.
  axis = oPoint;
  psiCoreEdge = psiCoreBoundary;
}

// Returns the values of psi at a physical location defined by pt.
double EqdskData::getPsiAtPoint(const Point& pt)
{
  double psi;
  int ierr;
  eval_field_val(&pt.x, &pt.y, &psi, &ierr, reversePsi);

  if (ierr)
    std::cout << "WARNING: psi value at location " << pt.x << " , " << pt.y << " could not be found\n"; 

  return psi;
}

// Returns the values of psi gradients at a physical location defined by pt.
std::array<double,3> EqdskData::getPsiGradAtPoint(const Point& pt)
{
  std::array <double,3> psiGrad;
  int ierr;
  eval_field_grad(&pt.x, &pt.y, &psiGrad[0], &ierr, reversePsi);
  psiGrad[2] = 0.0;  // working with 2D

  if (ierr)
    std::cout << "WARNING: psi gradient at location " << pt.x << " , " << pt.y << " could not be found\n";
  
  return psiGrad;
}

// Returns the absolute value of psi gradients at a physical location defined by pt.
double EqdskData::getPsiGradAbsoluteAtPoint(const Point& pt)
{
  double psiGradAbsolute;
  int ierr;
  eval_field_grad_abs2(&pt.x, &pt.y, &psiGradAbsolute, &ierr, reversePsi);
 
  if (ierr)  
    std::cout << "WARNING: psi gradient absolute at location " << pt.x << " , " << pt.y << " could not be found\n";

  return psiGradAbsolute;
}

// Returns the psi partial derivative at a physical location defined by pt.
double EqdskData::getPsiDerivativeAtPoint(const Point& pt, int dr, int dz)
{
  double dy2;
  int ierr;

  if (ierr)
    std::cout << "WARNING: psi derivative at location " << pt.x << " , " << pt.y << " could not be found\n";

  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &dy2, &ierr, reversePsi); 
  return dy2;
}

// Returns the values of poloidal current at given psi value.
double EqdskData::getCurrentAtPsi(double psi)
{
  double current;
  int ierr; 
  eval_i_val_(&psi, &current, &ierr);
 
  if (ierr)
    std::cout << "WARNING: current derivative at psi =  " << psi << " could not be found\n";

  return current;
}

//  Convert the normalized psi value to psi value.
double EqdskData::convertNormToPsi(double normPsi)
{
  double psiAxis = axis.getPsi(); 
  return normPsi*(psiCoreEdge - psiAxis) + psiAxis;
}

// Convert the psi value to normalized psi value.
double EqdskData::convertPsiToNorm(double psi)
{
  double psiAxis = axis.getPsi();
  return (psi - psiAxis)/(psiCoreEdge - psiAxis);
}

// Function to update point pt, to the nearest point with goalPsi.
bool EqdskData::snapToPsi(Point& pt, double goalPsi)
{
  // Step 1: Get the psi at the given point.
  double psi = getPsiAtPoint(pt);
  int numIter = 0;  // number of iterations

  // Step 2: Set the step size for Newton Method
  double maxStepMeters = 0.0025;

  // Step 3: Keep finding until the nearest point with goal psi
  // is found or iterations exceed max iterations (1000).
  while (fabs(psi - goalPsi) > psiTolerance)
  {
    // Step 3.1: Find the adjustments (deltas) in both directions.
    std::array<double,3> psiDeriv = getPsiGradAtPoint(pt);
    double psiDerivSquared = (psiDeriv[0]*psiDeriv[0]) + (psiDeriv[1]*psiDeriv[1]);
    double deltaX = psiDeriv[0]*(psi - goalPsi)/psiDerivSquared;
    double deltaY = psiDeriv[1]*(psi - goalPsi)/psiDerivSquared;
    double dist = sqrt(deltaX*deltaX + deltaY*deltaY);

    if (dist > maxStepMeters)
    {
      deltaX *= maxStepMeters/dist;
      deltaY *= maxStepMeters/dist;
    }

    // Step 3.2: Update the point based on deltas.
    pt.x -= deltaX;
    pt.y -= deltaY;

    // Step 3.3: If updated point is outside the domain, return false.
    if (!inDomain(pt, boundingBox))
      return false;

    // Step 3.4: Update psi and number of iterations.
    psi = getPsiAtPoint(pt);
    numIter++;

    if (numIter > 1000)
      return false;
  } 

  return true;
}

// Function to return magnetic field components at a physical location.
int EqdskData::magneticField(std::array <double,3> ptArray, std::vector <double>& dpsi, int dimension)
{
  // Step 1: Define the point pt based on the input dimension.
  Point pt;
  if (dimension == 2)
    pt = Point(ptArray[0], ptArray[1]);
  else
    pt = Point(ptArray[0], ptArray[1], ptArray[2]);

  // Step 2: If point is outside of the box, return 1.
  if (!insideBox(pt))
    return 1;

  // Step 3: get the psi value to calculate poloidal current, and gradients of the psi.
  double psi = getPsiAtPoint(pt);
  std::array<double,3> psiGrad = getPsiGradAtPoint(pt);
  double poloidalCurrent = getCurrentAtPsi(psi);
  
  // Step 4: Define magnetic field (B) components based on current and gradients of psi.
  // Br= - (1/R) dpsi/dZ , Bz = (1/R) dpsi/dR, Bphi = I/R
  dpsi.push_back(-psiGrad[1]/poloidalCurrent);
  dpsi.push_back(psiGrad[0]/poloidalCurrent);

  // Step 5: For 3D, normalize the third direction.
  if (dimension == 3)
    dpsi.push_back(1.0);

  return 0;
}

// Runge-Kutta method for numerical integration.
bool EqdskData::rk4(Point& point0, Point& point1, double dt, int dimension)
{
  // Step 1: Check validity of the dimension.
  if (dimension != 2 || dimension != 3)
  {
    std::cerr << "ERROR: Invalid dimension = " << dimension << " for RK4 method\n";
    std::cerr << "Valid dimensions are 2, 3\n";
    exit(1);
  }

  // Step 2: Setup points in terms of arrays (easy to update in a loop)
  std::array <double, 3> pt0 = {point0.x , point0.y, point0.z};
  std::array <double, 3> pt1 = {point1.x , point1.y, point1.z};

  // Step 3: Define k coefficients for Runge-Kutta method and based on them
  // update the pt1. Make sure to update as Point too not just array.
  std::vector <double> k1, k2, k3, k4;

  // Step 3.1: Evaluate k1 and update based on k1.
  if(magneticField(pt0, k1, dimension))
    return true;

  for(int i = 0; i < dimension; i++)
    pt1[i] = pt0[i] + k1[i]*dt/2.0;
  point1 = Point(pt1[0], pt1[1], pt1[2]);  // Point update

  // Step 3.2: Evaluate k2 and update based on k2.
  if(magneticField(pt1, k2, dimension))
    return true;

  for(int i = 0; i < dimension; i++)
    pt1[i] = pt0[i] + k2[i]*dt/2.0;
  point1 = Point(pt1[0], pt1[1], pt1[2]);  // Point update

  // Step 3.3: Evaluate k3 and update based on k3.
  if(magneticField(pt1, k3, dimension))
    return true;

  for(int i = 0; i < dimension; i++)
    pt1[i] = pt0[i] + k3[i]*dt;
  point1 = Point(pt1[0], pt1[1], pt1[2]);  // Point update

  // Step 3.4: Evaluate k4 and update based on k4.
  if(magneticField(pt1, k4, dimension))
    return true;

  for(int i = 0; i < dimension; i++)
    pt1[i] = pt0[i] + (k1[i] + 2*k2[i] + 2*k3[i] + k4[i])*dt/6.0;
  point1 = Point(pt1[0], pt1[1], pt1[2]);  // Point update

  // Step 3: Confirm if it is just a valid point within domain
  if (!insideBox(point1))
    return true;

  return false;
}

// To find a next point by going around on edges of box boundary when tracing 
// point hits a box boundary using Newton method.
bool EqdskData::findNextPsiPointOnBoundary(Point& pt, double psi, int side) 
{
  // Step 1: Get the psi value at the pt, and setup variables.
  double currentPsi =  getPsiAtPoint(pt);
  int numIterations = 0;

  // Step 2: Keep looking for the desired psi along the box until we are
  // within the tolerance. Or until hit maximum allowed iterations.
  while (currentPsi > psi + psiTolerance || currentPsi < psi - psiTolerance)
  {
    // Step 2.1: Define the order of derivatives based on the input side.
    // dr = 0,1 , dz= 0,1
    int dr = (side + 1)%2;
    int dz = side%2;
    double dpsi = getPsiDerivativeAtPoint(pt, dr, dz);

    // Step 2.2: Update x coordinate of the point pt.
    if(side%2 == 0)
    {
      pt.x += (psi - currentPsi)/dpsi;
      if(pt.x < boundingBox[0])
      {
        pt.x = boundingBox[0];
        side = 1;
      }
      if(pt.x > boundingBox[2])
      {
        pt.x = boundingBox[2];
        side = 3;
      }
    }
    else // Step 2.3: Update y coordinate of the point pt.
    {
      pt.y += (psi - currentPsi)/dpsi;
      if(pt.y < boundingBox[1])
      {
        pt.y = boundingBox[1];
        side = 2;
      }
      if(pt.y > boundingBox[3])
      {
        pt.y = boundingBox[3];
        side = 0;
      }
    }
    numIterations++;

    // Step 2.4: Update psi value and number of iterations.
    currentPsi = getPsiAtPoint(pt);
    assert(numIterations < 100);

    if (!insideBox(pt))
      return false;
  }
  return true;
}

// Given a target psi and an initial guess, find a point in direction of dir.
int EqdskData::findPsiPt(double targetPsi, Point startPoint, std::array<double,2> dir, Point& finalPoint) 
{
  // Step 1: Get the psi value at the initial guess pt, and setup variables. Also, check if point is
  // inside the box
  if (!insideBox(startPoint))
    return 0;

  double tolerance = 1e-5;
  int maxIterations = 100;
  double psi= getPsiAtPoint(startPoint);

  // Step 2: Define step size based on the length of domain box.
  double lenghtScaleOnR = 0.01*(boundingBox[2] - boundingBox[0]);
  int numIterations = 0;

  // Step 3: Keep looking for the desired point with psi value until the point is within
  // tolerance value or iterations don't reach the maximum allowed iterations.
  while (fabs(targetPsi - psi) > tolerance && numIterations++ < maxIterations)
  {
    std::array<double,3> gradPsi = getPsiGradAtPoint(startPoint);
    double lengthToMove = 1.0/(dir[0]*gradPsi[0] + dir[1]*gradPsi[1]);
    lengthToMove *= (targetPsi - psi);
    if (lengthToMove > lenghtScaleOnR)
      lengthToMove = lenghtScaleOnR;

    if (lengthToMove < -1.002*lenghtScaleOnR)  //1.002 to prevent from hopping back and forth
      lengthToMove = -1.002*lenghtScaleOnR; 

    startPoint.x += lengthToMove*dir[0];
    startPoint.y += lengthToMove*dir[1];
    psi = getPsiAtPoint(startPoint);
    if (!insideBox(startPoint))
      return 0;
  }

  // Step 4: Update the point
  finalPoint.x = startPoint.x;
  finalPoint.y = startPoint.y;
  
  // Step 5: Return 1 for success, 0 otherwise
  if (fabs(targetPsi - psi) < psiTolerance)
    return 1;
  else
    return 0;
}

// Given the psi value, this function finds the coordinates of the point on a
// horizontal line from axis to the box (either inward or outward).
Point EqdskData::convertPsiToPoint(double psi)
{
  std::array<double,2> dir = {1,0};
  Point initialGuess;
  double psiNorm = convertPsiToNorm(psi);

  if (inboardStart && psiNorm < 1.0)
    initialGuess = Point((axis.getPoint().x + boundingBox[0])*0.5, axis.getPoint().y);
  else if (!inboardStart || psiNorm > 1.0)
    initialGuess = Point((axis.getPoint().x + boundingBox[2])*0.5, axis.getPoint().y); 

  Point returnPt;
  int returnIndex = findPsiPt(psi, initialGuess, dir, returnPt);
  assert(returnIndex == 1);

  return returnPt;
}

// Function to get domain bounding box.
DomainBox EqdskData::getDomainBox()
{
  DomainBox box(boundingBox);
  return box;
}

bool EqdskData::insideBox(const Point& pt)
{
   // Step 1: Get the lower and upper limits of rectangular box.
  double xMin = boundingBox[0], yMin = boundingBox[1];
  double xMax = boundingBox[2], yMax = boundingBox[3];

  // Step 2: If point is outside the box set, return false.
  if (pt.x < xMin || pt.x > xMax || pt.y < yMin || pt.y > yMax)
    return false;

  return true;
}

bool EqdskData::insideBox(const std::array <double,3>& pt)
{
  Point point(pt[0], pt[1], pt[2]);
  return insideBox(point);
}

// Returns the number of poloidal planes (user input).
const int EqdskData::getNumPlanes() const
{
  return numPlanes;
}

// Returns step size in radian (user input).
const double EqdskData::getStepRadians() const
{
  return stepRadians;
}

// Returns psi tolerance for calcuations (user input).
const double EqdskData::getPsiTolerance() const
{
  return psiTolerance;
}

// Returns spacing tolerance (user input).
const double EqdskData::getSpacingToleranceOptimal() const
{
  return spacingToleranceOptimal;
}

// Returns struct FluxData containing input data of flux.
const FluxData& EqdskData::getFluxInputData() const
{
  return fluxInputData;
}

// Function to check if start of the flux curve is random or not.
const bool& EqdskData::randomStart() const
{
  return fluxRandomStart;
}

// Returns the type of desired intra curve spacing option.
const int& EqdskData::getIntraCurveSpacingOption() const
{
  return intraCurveSpacingOption;
}

// Returns the absolute spacing tolerance.
const double& EqdskData::getSpacingToleranceAbsolute() const
{
  return spacingToleranceAbsolute;
}

// Function to check if the small spacing variation is allowed or not.
const bool& EqdskData::getIntraCurveSpacingSmallVariation() const
{
  return intraCurveSpacingSmallVariation;
}
