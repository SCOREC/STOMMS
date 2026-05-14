#include "eqdskData.h"

/***********************************************/
// Class: EqdskData
/***********************************************/
EqdskData::EqdskData(const Inputs& input, const PlaneMetaData& planeData, const PhysicsPoint& oPoint, const double& psiCoreBoundary)
{
  // Step 1: Read properties of eqdsk file from the inputs
  setParameters(input);  // for input parameters
  InputData inputData = input.getInputData();
  fluxInputData = inputData.fd;
  planeMetaData = planeData; 

  // Step 2: Domain box bounds
  double bbox[4]; // min r, min z, max r, max z
  get_b_box_(bbox);
  boundingBox = {bbox[0], bbox[1], bbox[2], bbox[3]};

  // Step 3: Domain definition needs primary o and x points.
  axis = oPoint;
  psiCoreEdge = psiCoreBoundary;

  // Step 4: Set intra curve grad spacing for non-field following case.
  if (intraCurveSpacingOption == -2)
    setintraCurveSpacingGradPsi();
}

// Set intraCurveSpacingGradPsi vector in the class
void EqdskData::setintraCurveSpacingGradPsi()
{
 // Step 1: Get the normalized psi vector from input.
 fluxValues = fluxInputData.fluxInput;

 // Step 2: Iterate over the psi values and corresponding mesh
 // sizes (don't need mesh sizes here. Just making sure we are
 // iterating over correct container) 
 for( const auto &itr : fluxInputData.fluxMeshSize)
 {
   // Step 3: Get a point from the normalized psi value.
   double psiNorm = itr.first;
   double psi = convertNormToPsi(psiNorm);
   Point pt = convertPsiToPoint(psi);

   // Step 4: Set y coordinate of the pt to y of magnetic axis.
   pt.y = axis.getPoint().y;

   // Step 5: Find absolute grad value and save it to the vector.
   std::array<double, 3> gradPsi = getPsiGradAtPoint(pt); 
   double gradPsiAbs = sqrt(gradPsi[0]*gradPsi[0] + gradPsi[1]*gradPsi[1]);
   intraCurveSpacingGradPsi.push_back(gradPsiAbs); 
 }
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
  eval_field_deriv(&pt.x, &pt.y, &dr, &dz, &dy2, &ierr, reversePsi);

  if (ierr)
    std::cout << "WARNING: psi derivative at location " << pt.x << " , " << pt.y << " could not be found\n";

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
  dpsi.resize(dimension);
  dpsi[0] = -psiGrad[1]/poloidalCurrent;
  dpsi[1] = psiGrad[0]/poloidalCurrent;

  // Step 5: For 3D, normalize the third direction.
  if (dimension == 3)
    dpsi[2] = 1.0;

  return 0;
}

// Runge-Kutta method for numerical integration.
bool EqdskData::rk4(Point& point0, Point& point1, double dt, int dimension)
{
  // Step 1: Check validity of the dimension.
  if (dimension != 2 && dimension != 3)
  {
    std::cerr << "ERROR: Invalid dimension = " << dimension << " for RK4 method\n";
    std::cerr << "Valid dimensions are 2, 3\n";
    exit(1);
  }

  // Step 2: Setup points in terms of arrays (easy to update in a loop)
  std::array <double, 3> pt0 = {point0.x , point0.y, point0.z};
  std::array <double, 3> pt1;

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

  // Step 4: Confirm if it is just a valid point within domain
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

  double tolerance = 1e-15;
  int maxIterations = 100;
  double psi = getPsiAtPoint(startPoint);

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
    {
      std::cout << "The point for the psi = " << targetPsi << " on the inboard/outboard midPlane is outside the domain box\n";
      std::cout << "If working with zero Xpoint case, set a smaller value of lastClosedPsi\n";
      std::cout << "If see this error message again, the point is still outside the domain. so, set even smaller value\n";
      return 0;
    }
  }

  // Step 4: Update the point
  finalPoint.x = startPoint.x;
  finalPoint.y = startPoint.y;
  
  // Step 5: Return 1 for success, 0 otherwise
  if (fabs(targetPsi - psi) < psiTolerance)
    return 1;
  else
  {
    std::cout << "========== ERROR FINDING POINT FOR PSI ==========\n";
    std::cout << "The point for the psi = " << targetPsi << " on the inboard/outboard midPlane is outside the domain box.\n";
    std::cout << "Normalized psi value = " << convertPsiToNorm(targetPsi) << " is too large for the domain.\n";
    std::cout << "Try to get rid of this normalized psi value from the flux input file.\n";
    return 0;
  }
}

// Given a target psi and a line (defined by two points), find the point on the line with
// the target psi. The initial guess point is found using bisection method on the line.
int EqdskData::findPsiPtOnLine(double targetPsi, const Point& pt1, const Point pt2, Point& finalPoint)
{
  double tolerance = psiTolerance;
  int maxIter = 100;

  // Step 1: Evaluate a middle point of the given line as a start point (start)
  // and find psi value at it.
  Point start = {(pt1.x + pt2.x)*0.5, (pt1.y + pt2.y)*0.5};
  double psi =  getPsiAtPoint(start);

  // Step 2: Evaluate a relative vector and vector absolute length.
  std::array <double, 2> vecRelative = {pt2.x - pt1.x, pt2.y - pt1.y};
  double len = sqrt(vecRelative[0]*vecRelative[0] + vecRelative[1]*vecRelative[1]);

  // Step 3: Set direction vector.
  std::array <double,2> dir = {vecRelative[0]/len, vecRelative[1]/len};

  // Step 4: Keep iterating until we find target psi or exceed the max number
  // of iterations.
  int iter=0;
  while(fabs(psi - targetPsi) > tolerance && iter++ < maxIter) 
  {
    std::array <double,3> gradPsi = getPsiGradAtPoint(start);
    double startNorm = sqrt((start.x - pt1.x)*(start.x - pt1.x) + (start.y - pt1.y)*(start.y - pt1.y))/len;
    double dx = (targetPsi - psi)/(dir[0]*gradPsi[0] + dir[1]*gradPsi[1]);
    double dxNorm = dx/len;
    double nextNorm = startNorm + dxNorm;

    // limiting dx = std::max(-startNorm, std::min(1.-startNorm, dx));
    if(0. > nextNorm || 1. < nextNorm) 
    {  //one more step to check if the value exists in the line
      double boundaryNorm = startNorm + std::max(-startNorm, std::min(1. - startNorm, dxNorm));
      Point boundaryPos(boundaryNorm*len*dir[0] + pt1.x, boundaryNorm*len*dir[1] + pt1.y);
      double boundaryPsi = getPsiAtPoint(boundaryPos);
      std::array <double, 3> boundaryGradPsi = getPsiGradAtPoint(boundaryPos);

      double boundaryStartNorm = sqrt((boundaryPos.x - pt1.x)*(boundaryPos.x - pt1.x) + (boundaryPos.y - pt1.y)*(boundaryPos.y - pt1.y))/len;
      double boundaryDx = (targetPsi - boundaryPsi)/(dir[0]*boundaryGradPsi[0] + dir[1]*boundaryGradPsi[1]);
      double boundaryNextNorm = boundaryStartNorm + boundaryDx/len;
      if(0. > boundaryNextNorm || 1. < boundaryNextNorm)
        return 0;
     
      start.x   = boundaryPos.x;
      start.y   = boundaryPos.y;
      startNorm = boundaryStartNorm;
      dx        = boundaryDx;
      nextNorm  = boundaryNextNorm;
    }

    start.x += dx*dir[0];
    start.y += dx*dir[1];
    psi = getPsiAtPoint(start);
  }

  // Step 5: Set the fianl point and check if it is at target psi or not.
  finalPoint.x = start.x;
  finalPoint.y = start.y;
  if(fabs(psi - targetPsi) < tolerance) 
    return 1;
  else 
    return 0;
}

// Given the psi value, this function finds the coordinates of the point on a
// horizontal line from axis to the box (either inward or outward).
Point EqdskData::convertPsiToPoint(double psi)
{
  // Step 1: Convert psi to normalized psi, and set an initial unit vector. 
  std::array<double,2> dir = {1,0};
  double psiNorm = convertPsiToNorm(psi);

  // Step 2: Find an initial guess (halfway of midplane). Depending on the user's
  // input, its either on outboard midplane of inboard midplace.
  Point initialGuess;
  if (inboardStart && psiNorm < 1.0)
    initialGuess = Point((axis.getPoint().x + boundingBox[0])*0.5, axis.getPoint().y);
  else if (!inboardStart || psiNorm > 1.0)
    initialGuess = Point((axis.getPoint().x + boundingBox[2])*0.5, axis.getPoint().y); 

  // Step 3: Find point on the psi (still on horizontal line since given dir = [1,0]).
  Point returnPt;
  int returnIndex = findPsiPt(psi, initialGuess, dir, returnPt);
  assert(returnIndex == 1);

  return returnPt;
}

// Function to get spacing between the two psi values bounding the psiNorm.
// Not a good way to set mesh size on faces. It was in legacy TOMMS code.
// Needed this to match some tests during development. Should discard/come
// up with better method in future.
double EqdskData::getInterCurveSpacingLinear(double psiNorm)
{
  // Step 1: Get the normalized psi vector and get its low and high 
  // bounds and size.
  std::vector <double>& psiInputVector = fluxInputData.fluxInput;
  double psiNormMin = psiInputVector.front();
  double psiNormMax = psiInputVector.back();
  int nGrid = psiInputVector.size();
  assert (nGrid > 1);
  
  // Step 2: Find the index of nearest psi value from the vector. 
  int lowBound = nGrid - 2;
  for (int i = 0; i < nGrid - 1; i++)
  {
    if (psiNorm <= psiInputVector[i])
    {
      lowBound = i;
      break;
    }
  }

  // Step 3: Get the point on outboard or inboard midplane for lower and upper bound psi values.
  Point lowBoundPt = convertPsiToPoint(convertNormToPsi(psiInputVector[lowBound]));
  Point upBoundPt = convertPsiToPoint(convertNormToPsi(psiInputVector[lowBound+1]));

  // Step 4: Get the spacing between horizontal coordinates of two points.
  double spacing = fabs(lowBoundPt.x - upBoundPt.x);
  assert (spacing > 0.0);
  return spacing;
}

// Function to get domain bounding box.
DomainBox EqdskData::getDomainBox()
{
  DomainBox box(boundingBox);
  return box;
}

// Checks if a point pt is inside or outside of the bounding box.
// pt is given as Point.
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

// Checks if a point pt is inside or outside of the bounding box.
// pt is given as an array.
bool EqdskData::insideBox(const std::array <double,3>& pt)
{
  Point point(pt[0], pt[1], pt[2]);
  return insideBox(point);
}

// Function to get intra curve spacing for the given psi and a starting point.
double EqdskData::getNodeSpacing(const Point& pt, double psiNorm)
{
  // Step 1: Get node spacing from psi value.
  double meshSize = planeMetaData.getNodeSpacingAtFlux(psiNorm);

  // Step 2: For the non-field following option, readjust the size.
  if (intraCurveSpacingOption == -2)
  {
    // Step 2.1: Check bounds
    double tolerance = 1e-8;
    if (psiNorm < (fluxValues.front() - tolerance) || psiNorm > (fluxValues.back() + tolerance))
    {
      std::cerr << "ERROR: Given psi normalized value = " << psiNorm << " is out of bounds\n";
      std::cout << "The value should be in the following range: " << fluxValues.front() << " , " << fluxValues.back() << "\n";
      exit(1);
    }

    // Step 2.2: Get the index of the flux curve in the vector
    int indx = 0;
    while (fluxValues[indx] < psiNorm)
    {
      indx++;
      if (indx == intraCurveSpacingGradPsi.size())
        break;
    }

    // Step 2.3: If not out of bounds, find the index of first value in the fluxValues 
    // vector that is equal or greater than given psiNorm. First check if its on the
    // starting point of the vector, if yes return corresponding mesh size value
    double gradAbs;
    if (indx == 0)
      gradAbs = intraCurveSpacingGradPsi[0];
    else if (indx == intraCurveSpacingGradPsi.size())
      gradAbs = intraCurveSpacingGradPsi.back();
    else 
    {
      // Step 2.4: Linear Interpolation (y = (y2 - y1)/(x2 - x1)*(x - x1) + y1)
      double y1 = intraCurveSpacingGradPsi[indx - 1];
      double y2 = intraCurveSpacingGradPsi[indx];
      double x1 = fluxValues[indx - 1];;
      double x2 = fluxValues[indx];;
      gradAbs = (y2 - y1)/(x2 - x1)*(psiNorm - x1) + y1;
    }

    // Step 2.5: Update the distance based on intraCurveSpacingPropFactors.
    std::array<double, 3> gradPsi = getPsiGradAtPoint(pt);
    double gradPsiAbs = sqrt(gradPsi[0]*gradPsi[0] + gradPsi[1]*gradPsi[1]);
    meshSize = meshSize*std::min(intraCurveSpacingPropFacMax, std::max(intraCurveSpacingPropFacMin, gradAbs/gradPsiAbs));
  }
  return meshSize;
}

// Function to set eqdsk parameters from inputs.
void EqdskData::setParameters(const Inputs& in)
{
  reversePsi = in.useReversePsi();
  inboardStart = in.useInboardStart();
  fluxRandomStart = in.useFluxRandomStart();
  numPlanes = in.getNumTokamakPlanes();
  stepRadians = in.getStepRadians();
  psiTolerance = in.getPsiTolerance();
  spacingToleranceOptimal = in.getSpacingToleranceOptimal();
  spacingToleranceAbsolute = in.getSpacingToleranceAbsolute();
  intraCurveSpacingSmallVariation = in.useIntraCurveSpacingSmallVariation();
  zeroXptWall = in.useZeroXptWall();
  intraCurveSpacingOption = in.getIntraCurveSpacingOption();
  intraCurveMinLengthLastEdge = in.getIntraCurveMinLengthLastEdge();
  intraCurveSpacingPropFacMax = in.getIntraCurveSpacingPropFacMax();
  intraCurveSpacingPropFacMin = in.getIntraCurveSpacingPropFacMin();
  useWall = in.useWallCurve();
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

// Function to get minimum spacing between last two points of a flux curve. 
const double& EqdskData::getIntraCurveMinLengthLastEdge() const
{
  return intraCurveMinLengthLastEdge;
}

// Function to check if model needs to be bounded by wall curve or last closed flux curve.
const bool& EqdskData::useWallCurve() const
{
  return useWall;
}
