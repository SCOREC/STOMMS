#include "fluxCurveUtilityEqdsk.h"

// Given a point on curve, and properties of magnetic field along with the curve meta deta, 
// find next point on the poloidal curve. Not field following.
bool findNextPoint(Point& startPoint, Point& nextPoint, double& lengthPoloidalGoal, const PhysicsPoint& oPoint, 
                   CurveMetaData& curveData, EqdskData eqdsk)
{
  double stepToroidalUnit = getStepToroidalUnit(oPoint, lengthPoloidalGoal, eqdsk.getNumPlanes());
  double distNewToStartLast = 0.0;
  double lengthPoloidal = 0.0; // actual curve length in a poloidal plane obtained by sum
  double lengthTolerance = 1e-6;  // tracing tolerance
  const DomainBox& box = eqdsk.getDomainBox();

  nextPoint = startPoint;
  Point curveStart = curveData.origin;
  Point point2; // point to hold temporary values

  // Move the step
  int numIterations = 0;
  int maxIterationsAllowed = 100000;
  while (true)
  {
    int outOfDomain = 0;
    outOfDomain = eqdsk.rk4(nextPoint, point2, stepToroidalUnit, 2);
    if (outOfDomain)
    {
       int side = updatePointOnBoundary(point2, box);
       if (side < 0)
         return false;

       bool ok = eqdsk.findNextPsiPointOnBoundary(point2, curveData.psi, side);
       assert(ok);
      
       nextPoint = point2;
       return false;
    }
    numIterations++;

    // Step : Check if the psi is correct
    double dpsi = eqdsk.getPsiAtPoint(point2) - curveData.psi;
    if (fabs(dpsi) < eqdsk.getPsiTolerance())
    {
      bool adjustedPointToPsi = eqdsk.snapToPsi(point2, curveData.psi);
      assert(adjustedPointToPsi);
    }
    
    // Step : Calculate distance moved
    double distOldToNew = distance2D(nextPoint, point2);
    double lengthPoloidalCandidate = distOldToNew + lengthPoloidal;
    if(lengthPoloidalCandidate > lengthPoloidalGoal) 
    {
      stepToroidalUnit *= 0.5;
      continue;
    }
    else
      lengthPoloidal = lengthPoloidalCandidate;

    if (doesPointHitTheOrigin(nextPoint, point2, startPoint, lengthPoloidalGoal, distNewToStartLast, curveData, eqdsk))
      return true;
    distNewToStartLast = distance2D(point2, curveData.origin);

    if (numIterations > maxIterationsAllowed)
    {
      std::cerr << "In function " << __func__ << ": loop iteration is over 100k. i.e. hard to find next point on a flux curve. \n";
      exit(1);
    }

    nextPoint = point2;  // update the point
    if (stepToroidalUnit < lengthTolerance)
      return true;
  }
}

// Given a point on curve, and properties of magnetic field along with the curve meta deta, 
// find next point on the poloidal curve. Field following point tracing.
bool findNextFieldFollowingPoint(Point& startPoint, Point& nextPoint, double& dist, int m, CurveMetaData& curveData, 
                                 EqdskData eqdsk)
{
  int steps;
  double stepToroidalAngle = getStepToroidalAngle(eqdsk.getNumPlanes(), m, steps, eqdsk.getStepRadians());
  const DomainBox& box = eqdsk.getDomainBox();

  double goal = dist;
  dist = 0.0;
  double distNewToStartLast = 0.0;

  nextPoint = startPoint;
  Point curveStart = curveData.origin;
  Point point2; // point to hold temporary values

  // Move the step
  int numIterations = 0;
  int maxIterationsAllowed = 100000; 
  double stepToroidalMeter = 0.0;
  while (true)
  {
    stepToroidalMeter = nextPoint.x*stepToroidalAngle;
    int res = eqdsk.rk4(nextPoint, point2, stepToroidalMeter, 3);
    point2.z = 0.0;

    // Step : If point is outside the domain, try to find the point on the box boundary and return
    // true if the point founded is close to origin(start of the curve).
    if (res > 0)
      return isBoundaryPointOnOrigin(nextPoint, point2, dist, numIterations, curveData, eqdsk, box);

    numIterations++;


    // Step : Check if the psi is correct
    double dpsi = eqdsk.getPsiAtPoint(point2) - curveData.psi;
    if (fabs(dpsi) - eqdsk.getPsiTolerance())
    {
      bool adjustedPointToPsi = eqdsk.snapToPsi(point2, curveData.psi);
      assert(adjustedPointToPsi);
    }


    // Step : Calculate distance moved
    double distOldToNew = distance2D(nextPoint, point2);
    dist = dist + distOldToNew;
    if (doesPointHitTheOrigin(nextPoint, point2, startPoint, goal, distNewToStartLast, curveData, eqdsk))
      return true;
   
    distNewToStartLast = distance2D(point2, curveData.origin);

    if (numIterations > maxIterationsAllowed)
    {
      std::cerr << "In function " << __func__ << ": loop iteration is over 100k. i.e. hard to find next point on a flux curve. \n";
      exit(1); 
    }
    if (m == 0 && dist > goal)
      return true;

    steps--;
    if (steps == 0 && m > 0)
      return true;
  }
}

// To find the toroidal step size for rk4 method.
double getStepToroidalAngle(int numPlanes, int m, int& steps, double stepRadians)
{
  double pi = 3.14159265359;
  double phi = pi*2;

  phi = phi/numPlanes;
  if (m > 0)
    phi = phi/m;

  // calculate step size
  steps = phi/stepRadians;

  if(steps < 1) 
    steps = 1;

  // Calculate step toroidal angle
  // this step_tor_angle might be possible to cause a distance larger than intra_curve_spacing
  double stepToroidalAngle = phi/steps; 
  if(m == 0)
  {
    steps = 10/stepRadians;
    stepToroidalAngle = stepRadians;
  }
  
  return stepToroidalAngle;
}

// To find normalized toroidal step for rk4 method.
double getStepToroidalUnit(const PhysicsPoint& oPoint, double goal, int numPlanes)
{
  // Set up variables needed in the calculation.
  double subStepMaxBound = 100.00;

  // toridalMaxBound = (2*pi*R_axis)/(# of poloidal planes*subStepMaxBound)
  double pi = 3.14159265359;
  Point axis = oPoint.getPoint();
  double toridalMaxBound = (2.0*pi*axis.x)/(numPlanes*subStepMaxBound);
  double stepToroidalUnit = std::min(goal, toridalMaxBound);

  return stepToroidalUnit;
}

// If a point is outside the bounding box, readjust it on the boundary.
// Update point pt on the edges of bounding box and return the side of the box its on.
// side = 1 is left, side = 3 is right, side = 2 is bottom, side = 0 is top.
int updatePointOnBoundary(Point& pt, const DomainBox& box)
{
  int side = -1;

  // Step 1. Check if point is on the left of the bounding box.
  // If yes, update the point, and set side to left.
  if (pt.x < box.xMin)
  { 
    side = 1;
    pt.x = box.xMin;
  }

  // Step 2. Check if point is on the right of the bounding box.
  // If yes, update the point, and set side to right.
  if (pt.x > box.xMax)
  {// 
    side = 3;
    pt.x = box.xMax;
  }

  // Step 3. Check if point is on the bottom side of the bounding box.
  // If yes, update the point, and set side to bottom.
  if (pt.y < box.yMin)
  { 
    side = 2;
    pt.y = box.yMin;
  }

  // Step 4. Check if point is on the top side of the bounding box.
  // If yes, update the point, and set side to top.
  if (pt.y > box.yMax)
  {// 
    side = 0;
    pt.y = box.xMax;
  }

  return side;
}

// Check if the point adjusted on the bounding box, hits the origin (starting point) of curve or not.
bool isBoundaryPointOnOrigin(Point& pt1, Point& pt2, double& dist, int numIterations, 
                             CurveMetaData& curveData, EqdskData& eqdsk, const DomainBox& box)
{
  int side = updatePointOnBoundary(pt2, box);
  if (side < 0)
    return false;

  bool ok = eqdsk.findNextPsiPointOnBoundary(pt2, curveData.psi, side);
  assert(ok);

  double distPtToPt = distance2D(pt1, pt2);
  dist = dist + distPtToPt;
  pt1 = pt2;
  double distStarttoPt = distance2D(pt1, curveData.origin);
  if (numIterations != 0 && distStarttoPt < distPtToPt)
  {
    curveData.hitOrigin = true;
    return true;      
  }
  
  return false;
}

// Check if the next traced points, hits the origin (starting point) of curve or not.
bool doesPointHitTheOrigin(Point& pt1, Point& pt2, Point startPt, double goal, double distNewToStartLast, 
                           CurveMetaData& curveData, EqdskData& eqdsk)
{
  double distNewToStart = distance2D(pt2, curveData.origin);
  double distSourceToNew = distance2D(startPt, pt2);
  
  pt1 = pt2; // update the point.

  if (distNewToStart < distSourceToNew || (distNewToStart < distNewToStartLast &&
      curveData.xPoint && distNewToStart < goal*(1.0+ eqdsk.getSpacingToleranceOptimal())))
  {
    curveData.hitOrigin = true;
    pt1.x = 0.5*(startPt.x + curveData.origin.x);
    pt1.y = 0.5*(startPt.y + curveData.origin.y);
    pt1.z = 0.5*(startPt.z + curveData.origin.z);

    double dpsi = eqdsk.getPsiAtPoint(pt1) - curveData.psi;
    if (fabs(dpsi) - eqdsk.getPsiTolerance())
    {
      bool adjustedPointToPsi = eqdsk.snapToPsi(pt1, curveData.psi);
      assert(adjustedPointToPsi);
    }

    return true;
  } 

  return false;
}

std::vector <PhysicsPoint> getStartPointClosed(const std::vector <double>& corePsiValues, EqdskData& eqdskData)
{
  std::vector <PhysicsPoint> startPoints;
  for (int i = 0; i < corePsiValues.size(); i++)
  {
    double psi = corePsiValues[i];
    double psiNormalized = eqdskData.convertPsiToNorm(psi);
    assert (psiNormalized >= 0.0);

    Point pt = eqdskData.convertPsiToPoint(psi);
    PhysicsPoint startPoint(pt, psi, PointType::None);
    startPoints.push_back(startPoint);
  }

  return startPoints;
}

std::vector <std::vector <PhysicsPoint>> getStartPointsOnSimFace(pGFace face, const std::vector<double>& psiNormList, EqdskData& eqdskData) 
{
  std::vector <std::vector <PhysicsPoint>> startPoints;
  startPoints.resize(psiNormList.size());

  pPList edges = GF_edges(face);
  std::vector<double> psiToAvoid;

  double tempPsi;
  for (int i = 0; i < PList_size(edges); ++i) 
  {
    pGEdge edge = static_cast<pGEdge>(PList_item(edges,i));
    if(GEN_numNativeDoubleAttribute(edge, "PsiNorm")) 
    {
      GEN_nativeDoubleAttribute(edge, "PsiNorm", &tempPsi);
      psiToAvoid.push_back(tempPsi);
    }
  }
  
  // get vertices of flux edges - a flux curve consists of one model edge
  std::vector <Point> pointsToAvoid;
  for(int i = 0; i < PList_size(edges); ++i) 
  {
    pGEdge edge = static_cast<pGEdge>(PList_item(edges,i));
    if(!GEN_numNativeDoubleAttribute(edge, "psi")) 
      continue; //we are looking for a flux curve

    pPList vertices = GE_vertices(edge);

    pGVertex v0 = GE_vertex(edge, 0);
    pGVertex v1 = GE_vertex(edge, 1);
    assert(v0 && v1);

    std::array <double, 3> pos0, pos1;
    GV_point(v0, pos0.data());
    GV_point(v1, pos1.data());
    Point pt1(pos0[0], pos0[1]);
    Point pt2(pos1[0], pos1[1]);
    pointsToAvoid.push_back(pt1);
    pointsToAvoid.push_back(pt2);
  }

  int nFoundGlobal = 0;
  //  SimModSuite APIs : non threadsafe
  for (int i = 0; i < PList_size(edges); ++i) 
  {
    pGEdge edge = static_cast<pGEdge>(PList_item(edges,i));
    if(GEN_numNativeDoubleAttribute(edge, "psiNorm")) 
      continue; //we are looking for a wall edge

    pPList vertices = GE_vertices(edge);

    pGVertex v0 = GE_vertex(edge, 0);
    pGVertex v1 = GE_vertex(edge, 1);
    assert(v0 && v1);
    std::array <double, 3> pos0, pos1;
    GV_point(v0, pos0.data());
    GV_point(v1, pos1.data());

    double dx = pos1[0] - pos0[0];
    double dy = pos1[1] - pos0[1];
    double edgeLength = GE_length(edge);
    double distEdge = sqrt(dx*dx + dy*dy);
    Point pt1(pos0[0], pos0[1]);
    Point pt2(pos1[0], pos1[1]);
    int nSample;
 
    #pragma omp parallel for schedule(dynamic)
    for(int ipsi = 0; ipsi < psiNormList.size(); ++ipsi) 
    {
      double psiNormalized = psiNormList[ipsi];
      double psi = eqdskData.convertNormToPsi(psiNormalized);
      
      bool psiFlux = false;
      for (int iflx = 0; iflx < psiToAvoid.size(); ++iflx) 
      {
        if(fabs(psiToAvoid[iflx] - psiNormalized) < 1e-6) 
        {
          psiFlux = true;
          break;
        }
      }
      if(psiFlux) 
        continue;
  
      std::vector <Point> ptFoundLocal = findPointBySectioningOnEdge(psi, edge, eqdskData);
      for(int j = 0; j < ptFoundLocal.size(); ++j) 
      {
        if(psiFlux) 
        {  //check if found point is a same with a vertex of a flux curve edge
          bool foundDuplicated = false;
          for (int iptAvoid = 0; iptAvoid < pointsToAvoid.size(); ++iptAvoid) 
          {
            Point posFound = ptFoundLocal[j];
            Point posAvoid = pointsToAvoid[iptAvoid];
            double dx = posFound.x - posAvoid.x;
            double dy = posFound.y - posAvoid.y;
            double dist = sqrt(dx*dx + dy*dy);
            if(dist< 1e-6) 
            {
              foundDuplicated = true;
              break;
            }
          }
          if(foundDuplicated)
            continue;
        }
        double psi = eqdskData.convertNormToPsi(psiNormalized);
        PhysicsPoint startPt(ptFoundLocal[j], psi, PointType::None);
        startPoints[ipsi].push_back(startPt);
      }
    }
    PList_delete(vertices);
  }
  PList_delete(edges);

  return startPoints;
}

int findStartPointIndexAtIntersection(const Point& pt1, const Point& pt2, std::vector<Point>& startPoints, int iFilter) 
{
  // Note that small number may cause some problems here since magnetic field 
  // is not a straight line, and an error can be accumulated during a trace.
  double toleranceDistanceFromSegment = 0.01; 

  int intersect = -1;
  double pMin = DBL_MAX;

  for(int i=0; i<startPoints.size(); i++) 
  {
    Point pt = startPoints[i];
    if(iFilter == i) 
      continue;
    if(toleranceDistanceFromSegment < fabs(distanceLineToPoint(pt1, pt2, pt))) 
      continue;

    double pCand = getParamatricCoordinate(pt1, pt2, pt);
    if(pCand >= 0.0 && pCand <= 1.0 && pCand < pMin) 
    {
      pMin = pCand;
      intersect = i;
    }
  }

  return intersect;
}


int intersectBetweenTwoLineSegments(const Point& testPt1, const Point& testPt2, const Point& refPt1, const Point& refPt2,
                                    double toleranceMeter, double toleranceSine, Point& intersectPt1, Point& intersectPt2)
{
  std::array <double,2> u = {testPt2.x - testPt1.x, testPt2.y - testPt1.y};
  std::array <double,2> v = {refPt2.x - refPt1.x, refPt2.y - refPt1.y};
  double du = sqrt(u[0]*u[0]+u[1]*u[1]);
  double dv = sqrt(v[0]*v[0]+v[1]*v[1]);
  assert( du > toleranceMeter && dv > toleranceMeter);

  std::array <double,2> w = {testPt1.x - refPt1.x, testPt1.y - refPt1.y};
  double D = u[0]*v[1] - u[1]*v[0];

  // test if they are parallel
  if (fabs(D)/(du*dv) < toleranceSine) 
  {           // S1 and S2 are parallel
    double perpUW = u[0]*w[1] - u[1]*w[0];
    double perpVW = v[0]*w[1] - v[1]*w[0];

    // parallel lines are not within tolernace in perpendicular direction
    if (fabs(perpUW/du) > toleranceMeter || fabs(perpVW>dv) > toleranceMeter)  
      return 0;                    // they are NOT collinear

    // they are collinear segments - get  overlap (or not)
    double t0, t1;                    // endpoints of S1 in eqn for S2
    std::array <double,2>  w2= {testPt2.x - refPt1.x, testPt2.y - refPt1.y};

    if (v[0] != 0.) 
    {
      t0 = w[0] / v[0];
      t1 = w2[0] / v[0];
    } 
    else 
    {
      t0 = w[1] / v[1];
      t1 = w2[1] / v[1];
    }

    // must have t0 smaller than t1 (just sorting)
    if (t0 > t1)
    { 
      double t = t0; 
      t0 = t1; 
      t1 = t;    // swap if not
    }
    double toleranceRelativeRef = toleranceMeter / dv;

    if (t0 > 1. + toleranceMeter || t1 < -toleranceMeter) 
      return 0;      // NO overlap

    t0 = std::min( 1., std::max( 0., t0 ) ); // Force the intersection point to be returned is exactly within on the reference line
    t1 = std::min( 1., std::max( 0., t1 ) ); // Force the intersection point to be returned is exactly within on the reference line
    if (t0 == t1) 
    {                  // intersect is a point
      intersectPt1.x = refPt1.x +  t0 * v[0];
      intersectPt1.y = refPt1.y +  t0 * v[1];
      return 1;
    }

    // they overlap in a valid subsegment
    intersectPt1.x = refPt1.x +  t0 * v[0];
    intersectPt1.y = refPt1.y +  t0 * v[1];
    intersectPt2.x = refPt1.x +  t1 * v[0];
    intersectPt2.y = refPt1.y +  t1 * v[1];
    return 2;
  }

  // the segments are skew and may intersect in a point
  // get the intersect parameter for S1 (test)
  double perpVW = v[0]*w[1] - v[1]*w[0];
  double toleranceRelativeTest = toleranceMeter / du;
  double sI = perpVW / D;
  if (sI < -toleranceRelativeTest || sI > 1. + toleranceRelativeTest)  // no intersect with S1
        return 0;

  // get the intersect parameter for S2 (ref)
  double perpUW = u[0]*w[1] - u[1]*w[0];
  double toleranceRelativeRef = toleranceMeter / dv;
  double tI = perpUW / D;
  if (tI < -toleranceRelativeRef || tI > 1. + toleranceRelativeRef)  // no intersect with S2
    return 0;

  tI = std::min( 1., std::max(0., tI) );
  intersectPt1.x = refPt1.x +  tI * v[0];
  intersectPt1.y = refPt1.y +  tI * v[1];
  return 1;
}

int getNumIntersection(const Point& pt1, const Point& pt2, const WallCurve& wall)
{
  double toleranceSine = 1e-8;
  double toleranceMeter = 1e-4;

  int numIntersections = 0;
  Point intersectionPoint1, intersectionPoint2;

  std::vector <Point> wallPoints = wall.getPoints();
  for(int i = 0; i < wallPoints.size() -1; i++)
    numIntersections += intersectBetweenTwoLineSegments(wallPoints[i], wallPoints[i+1], pt1, pt2, 0., toleranceSine, intersectionPoint1, intersectionPoint2);

  return numIntersections;
}

// Traverse a circle around the Xpt and find all the points with psi value equal to psi value of Xpt.
// This method will find all the possible points with the desired psi value on the circle.
std::vector <Point> getPushedPoints(const PhysicsPoint& xPoint, double dist, EqdskData& eqdsk)
{
  std::vector <Point> startPoints;
  double stepSize = 0.5;  // angle in degress. A size of 0.5 will result 720 scan points on the circle.
  double angle = 0.0;   // starting angle.
  double r = dist;    // intra_curve_spaing on the separatric curve
  Point point, ptPrev;
  double pi = 3.14159265359;
  Point xpt = xPoint.getPoint();
  double psi = xPoint.getPsi();

  int numIterations = 0;
  double dir1 = 0.0;
  double dir2 = 0.0;
  while (angle <= 360)
  {
    point.x = xpt.x + r*cos(angle*(pi/180));
    point.y = xpt.y + r*sin(angle*(pi/180));
    Point pt(point.x, point.y, 0.0);
    double psiPt = eqdsk.getPsiAtPoint(pt);
    if (numIterations > 0) // Go to this loop in second numIterationsation.
    {
      double psiPrev = eqdsk.getPsiAtPoint(ptPrev);
      psiPrev < psi ? dir1 = -1 : dir1 = 1;
      psiPt < psi ? dir2 = -1 : dir2 = 1;
      if (dir1*dir2 == -1)  // The desired point in between previous point and current point
      {
        double localStepSize = stepSize/2;  // Keep splitting the angular step size until we get desired point.
        double localAngle = angle - localStepSize;
        Point ptPrevTemp(ptPrev.x , ptPrev.y, 0.0);
        Point ptTemp(pt.x, pt.y, 0.0);
        Point ptCheck(xpt.x + r*cos(localAngle*(pi/180)), xpt.y + r*sin(localAngle*(pi/180)), 0.0);
        double psiCheck = eqdsk.getPsiAtPoint(ptCheck);
        double dir = 0;
        while (fabs(psiCheck - psi) > 1e-8)
        {
          localStepSize = localStepSize/2;
          psiCheck < psi ? dir = -1 : dir = 1;
          if (dir1*dir == -1)
          {
            ptTemp.x = ptCheck.x;
            ptTemp.y = ptCheck.y;
            localAngle = localAngle - localStepSize;
          }     
          if (dir2*dir == -1)
          {
            ptPrevTemp.x = ptCheck.x;
            ptPrevTemp.y = ptCheck.y;
            localAngle = localAngle + localStepSize;    
          }
          ptCheck.x = xpt.x + r*cos(localAngle*(pi/180));
          ptCheck.y = xpt.y + r*sin(localAngle*(pi/180));
          psiCheck = eqdsk.getPsiAtPoint(ptCheck);
    
        }   
        startPoints.push_back(ptCheck);
      }         
    }
    angle += stepSize;
    ptPrev.x = pt.x;
    ptPrev.y = pt.y;
    ptPrev.z = 0.0;
    numIterations++;
  }

  // Check if startspoint is toward or outgoing from a x-point
  for(int i=3; i>=0; i--) 
  {
    std::array <double,2> vec= {startPoints[i].x - xpt.x, startPoints[i].y - xpt.y};
    std::array <double,3> startPt = {startPoints[i].x, startPoints[i].y, 0.0}; 
    std::vector <double> bVec;
    int err = eqdsk.magneticField(startPt, bVec,2);
    assert(!err);
    if(vec[0]*bVec[0] + vec[1]*bVec[1] <= 0.)
      startPoints.erase(startPoints.begin()+i);
  }

  return startPoints;
}

std::vector <Point> findPointBySectioningBtwTwoPts(double targetPsi, int sampleN, const Point& pt1, const Point pt2, EqdskData& eqdsk) 
{
  std::vector <Point> pointsFound;
  std::vector <double> samplePsi;

  for(int jsample=0;  jsample < sampleN; jsample++) 
  {
    Point sampleX(pt1.x + (pt2.x - pt1.x)*jsample/(sampleN - 1), pt1.y + (pt2.y - pt1.y)*jsample/(sampleN - 1));
    double psi = eqdsk.getPsiAtPoint(sampleX);
    samplePsi.push_back(psi);
  } //sampling

  for(int jsample=0; jsample<sampleN-1; jsample++) 
  {
    Point ptVtx1, ptVtx2;
    ptVtx1.x = pt1.x+(pt2.x - pt1.x)*jsample/(sampleN-1);
    ptVtx1.y = pt1.y+(pt2.y - pt1.y)*jsample/(sampleN-1);
    ptVtx2.x = pt1.x+(pt2.x - pt1.x)*(jsample+1)/(sampleN-1);
    ptVtx2.y = pt1.y+(pt2.y - pt1.y)*(jsample+1)/(sampleN-1);
    Point vtxFound;

    if(((targetPsi - samplePsi[jsample])*(targetPsi - samplePsi[jsample+1]) <= 0.0)) 
    {
      if (eqdsk.findPsiPtOnLine(targetPsi, ptVtx1, ptVtx2, vtxFound)) 
        pointsFound.push_back(vtxFound); 
      else 
      {
        std::cout << "[CRITICAL] mathematically, the point is inside of the given section, but section search wasn't able to find it.\n";
        exit(1);
      }
    }
  }

  return pointsFound;
}

std::vector <Point> findPointBySectioningOnEdge(double targetPsi, pGEdge ge, EqdskData& eqdsk) 
{
  std::vector <Point> pointsFound;

  double stepSize = 1e-5;
  int minSamples = 10;
  int maxSamples = 10000;

  int sampleN = static_cast<int>(GE_length(ge)/ stepSize) + 1;
  sampleN = std::max(sampleN, minSamples);
  sampleN = std::min(sampleN, maxSamples);
  
  std::vector <double> samplePsi;
  double parR[2];
  GE_parRange(ge, &parR[0], &parR[1]);
  double parInterval = (parR[1] - parR[0])/(sampleN-1);
  for(int jsample=0;  jsample<sampleN; jsample++) 
  {
    double par = parR[0] + (jsample*parInterval);
    double pt[3];
    GE_point(ge, par, pt);
    Point sampleX(pt[0], pt[1]);
    double psi = eqdsk.getPsiAtPoint(sampleX);
    samplePsi.push_back(psi);
  } //sampling

  for(int jsample=0; jsample<sampleN-1; jsample++) 
  {
    std::array <double, 3> pt_vtx1, pt_vtx2;
    double par1 = parR[0] + (jsample*parInterval);
    double par2 = parR[0] + ((jsample+1)*parInterval);
    GE_point(ge, par1, pt_vtx1.data());
    GE_point(ge, par2, pt_vtx2.data());
    Point ptVtx1(pt_vtx1[0], pt_vtx1[1]); 
    Point ptVtx2(pt_vtx2[0], pt_vtx2[1]);

    Point vtxFound;

    if(((targetPsi - samplePsi[jsample])*(targetPsi - samplePsi[jsample+1]) <= 0.0)) 
    {
      if (eqdsk.findPsiPtOnLine(targetPsi, ptVtx1, ptVtx2, vtxFound)) 
        pointsFound.push_back(vtxFound); 
      else 
      {
        std::cout << "[CRITICAL] mathematically, the point is inside of the given section, but section search wasn't able to find it.\n";
        exit(1);
      }
    }
  }

  return pointsFound;
}

std::vector <Point> findStartPointOnWall(double psiNormalized, int type, const WallCurve& wall, EqdskData& eqdsk) 
{
  // search parameter
  double distSampling = 1e-2; // 1cm

  double psi = eqdsk.convertNormToPsi(psiNormalized);
  std::vector <Point> wallPoints = wall.getPoints();
  double nPts = wallPoints.size();

   std::vector <Point> startPoints;
  for(int i = 0; i < nPts-1; i++) 
  {  //wall edges
    double dx = wallPoints[i+1].x - wallPoints[i].x;
    double dy = wallPoints[i+1].y - wallPoints[i].y;
  
    Point pt1 = wallPoints[i];
    Point pt2 = wallPoints[i+1];
    double distEdge = sqrt(dx*dx + dy*dy);
    int nSample = 1 + std::max(1, static_cast<int>(distEdge/distSampling));
    std::vector <Point> ptFoundLocal = findPointBySectioningBtwTwoPts(psi, nSample, pt1, pt2, eqdsk);
    for(int j = 0; j < ptFoundLocal.size(); j++) 
    {
      if (startPoints.size() > 0 )
      {
        double deltaX = startPoints[startPoints.size()-1].x - ptFoundLocal[j].x;
        double deltaY = startPoints[startPoints.size()-1].y - ptFoundLocal[j].y;
        double dist = sqrt(deltaX*deltaX + deltaY*deltaY);
        if (dist < distSampling)
          continue;
      }
      startPoints.push_back(ptFoundLocal[j]);
    }
  }
  return startPoints;
}

bool validOpenCurve(const Flux& f, const std::vector <pGEdge>& edgesOnCurve)
{
  int numEdges = edgesOnCurve.size();
  int numPoints = f.fieldPoints.size();

  for (int i = 0; i < numPoints; i++)
  {
    Point pt = f.fieldPoints[i];
    bool isPointOnCurve = isPtOnCurve(pt, edgesOnCurve);
    if (!isPointOnCurve)
      return true;
  }

  // If all points are on wall curve, then check if they are on the same line
  // if yes, curve is not valid and return false.
  // If not, return true.
  for (int i = 0 ; i < numEdges; i++)
  {
    pGEdge ge = edgesOnCurve[i];
    int numPtsOnModelEdges = 0;
    for (int j = 0; j < numPoints; j++)
    {
      Point pt = f.fieldPoints[j];
      bool ptOnEdge = isPtOnModelEdge(pt, ge);
      if (ptOnEdge)
        numPtsOnModelEdges++;
    }
    if (numPtsOnModelEdges == numPoints)
      return false;
  }
 
  return true;
}

void restrictDistanceOfLastEdge(Flux& f, EqdskData& eqdsk)
{
  if (eqdsk.getIntraCurveMinLengthLastEdge() <= 0.0)
    return; 

  int nPts = f.fieldPoints.size();
  if (nPts <= 2)
    return;

  double minUserDefined = eqdsk.getIntraCurveMinLengthLastEdge();
  Point ptLast = f.fieldPoints[nPts-1];
  Point ptSecondLast = f.fieldPoints[nPts-2];

  double distLastEdge = distance2D(ptLast, ptSecondLast);
  if (minUserDefined > distLastEdge)
    f.fieldPoints.erase(f.fieldPoints.begin() + (nPts-2));
}
