#include "genFluxCurvesEqdsk.h"

// Closed curve generation top-level function
std::vector <Flux> genClosedFluxCurves(const std::vector <double>& corePsiValues, const PhysicsPoint& oPoint, 
                                       EqdskData& eqdskData, const PlaneMetaData& planeMetaData)
{
  std::vector <PhysicsPoint> startPoints = getStartPointClosed(corePsiValues, eqdskData);
  std::vector <Flux> closedFluxCurves;
  for (int i = 0; i < startPoints.size(); i++)
  {
    unsigned int mySeed = 1024 + i +1; // for random start
    PhysicsPoint startPoint = startPoints[i];
    ClosedFluxCurve closedFluxCurve(startPoint, mySeed, oPoint, eqdskData, planeMetaData);
    Flux fluxCurve = closedFluxCurve.getFluxCurve();
    fluxCurve.curveType = CurveType::Closed;
    closedFluxCurves.push_back(fluxCurve);
  }

  return closedFluxCurves;
}

// Separatrix generation top-level function
std::vector <Flux> genSeparatrixCurves(const std::vector <PhysicsPoint>& xPts, EqdskData& eqdskData, const WallCurve& wall, const PlaneMetaData& planeMetaData)
{
  std::vector <Flux> separatrices;
  std::map <int, std::vector<Point>> startPointsMap;
  for (int i = 0; i < xPts.size(); i++)
  {
    double psiNormalized = eqdskData.convertPsiToNorm(xPts[i].getPsi());
    int type = i+2;  //don't know why we even need this. review this later while proper cleanup.
    std::vector <Point> startPoints = findStartPointOnWall(psiNormalized, type, wall, eqdskData);
    assert (startPoints.size()%2 == 0);
    startPointsMap[i] = startPoints;     
  }

  std::vector <double> psiInputVector = eqdskData.getFluxInputData().fluxInput;
  for (int i = 0; i < xPts.size(); i++)
  {
    double psiNormalized = eqdskData.convertPsiToNorm(xPts[i].getPsi());
    std::vector <Point> startPoints = startPointsMap.at(i);
    if(psiInputVector.back() >= psiNormalized && psiInputVector.front() <= psiNormalized)
    {
      std::cout << "Generating separatrix curves from X-point # " << i+1 << "\n";
      SeparatrixCurve separatrixCurve(xPts[i], startPoints, eqdskData, wall, planeMetaData);
      std::vector <Flux> sepCurvesFromXpt = separatrixCurve.getFluxCurves();
      separatrices.insert(separatrices.end(), sepCurvesFromXpt.begin(), sepCurvesFromXpt.end());
    }
  }  
  return separatrices;
}

// Open flux curves top-level function
std::vector <Flux> genOpenFluxCurves(pGModel& model, EqdskData& eqdskData, const PhysicsPoint& oPoint,
                                     const WallCurve& wall, const PlaneMetaData& planeMetaData)
{
  std::cout << ".......... Generating Open Curves\n";

  // Step 1: Get SOL model faces to insert open curves on 
  // them with psi values for flux curves.
  std::vector <pGFace> solFaces = getSOLModelFaces(model);
  std::vector <double> psiValues = planeMetaData.getPlaneFluxValues();
  std::vector <Flux> openCurves;  // Save all open curves here.

  // Step 2: Generate flux curves on the SOL faces.
  for (int i = 0; i < solFaces.size(); i++)
  {
    pGFace gf = solFaces[i];
    std::vector <Flux> solCurves = getOpenCurvesOnFace(gf, psiValues, eqdskData, wall, oPoint, planeMetaData);
    openCurves.insert(openCurves.end(), solCurves.begin(), solCurves.end());
  } 

  // Step 3: Get the private faces and generate flux curves on them.
  bool fluxCurvesInPrivate = true;  // Get this parameter for user
  if (fluxCurvesInPrivate)
  {
    std::vector <pGFace> pvtFaces = getPrivateModelFaces(model);
    for (int i = 0; i < pvtFaces.size(); i++)
    {
      pGFace gf = pvtFaces[i];
      std::vector <double> psiValuesAtPvt = getSpacingOnPrivateRegion(gf, planeMetaData);
      std::vector <Flux> pvtCurves = getOpenCurvesOnFace(gf, psiValuesAtPvt, eqdskData, wall, oPoint, planeMetaData);
      openCurves.insert(openCurves.end(), pvtCurves.begin(), pvtCurves.end());
    }  
  }

  return openCurves;
}
/***********************************************/
// Class ClosedCurve
/***********************************************/
ClosedFluxCurve::ClosedFluxCurve(const PhysicsPoint& startPt, unsigned int& mySeed, const PhysicsPoint& oPoint, EqdskData& eqdskData, 
                                 const PlaneMetaData& planeMetaData):seed(mySeed), eqdsk(eqdskData)
{
  DomainBox box = eqdskData.getDomainBox();
  magneticAxis = oPoint;
  pMetaData = planeMetaData;
  Point startPoint = startPt.getPoint();
  psiNorm = eqdskData.convertPsiToNorm(startPt.getPsi());
  f.psiNormOnFlux = psiNorm;
  f.fieldPoints.push_back(startPoint);
  curveData.psi = startPt.getPsi();
  curveData.origin = startPoint;  

  Point nextPoint = startPoint;
  if (eqdsk.randomStart())
  {
    for (int i=0; i < 5; i++)    // Just prime the RNG a bit
      rand_r(&seed);    
  }

  while (true)
  {
    if (eqdsk.getIntraCurveSpacingOption() == -2)
      intersect = nonFieldFollowingCase(startPoint, nextPoint);
    else 
      intersect = fieldFollowingCase(startPoint, nextPoint);    

    if(eqdsk.randomStart() && tagStartingPoint) 
    {
      //random start was done. now  normal intra_curve_spacing
      f.fieldPoints.pop_back();
      curveData.origin = nextPoint;
      m=1;
    }
    tagStartingPoint = false;
    mChanged = false;

    f.fieldPoints.push_back(nextPoint);
    startPoint = nextPoint;

    if (curveData.hitOrigin)
    {
      f.fieldPoints.push_back(f.fieldPoints[0]);
      break;
    }
    if(intersect) 
    {
      std::cout << "This core curves hit box boundary, which is not ready to be used. exit." << "\n";
      assert(0);
      break;
    } // intersect = true
  } // while ends
}

bool ClosedFluxCurve::nonFieldFollowingCase(Point& startPoint, Point& nextPoint)
{
  randomFactor = 1.0;
  distanceSet = pMetaData.getNodeSpacingAtFlux(psiNorm);  

  if (eqdsk.randomStart() && !randomGen)
  {
    randomFactor = double(rand_r(&seed)%100000 + 1)/100000;
    randomGen = true;
  }

  distanceSet = distanceSet*randomFactor;
  distance = distanceSet;

  curveData.hitOrigin = false;
  intersect = !findNextPoint(startPoint, nextPoint, distance, magneticAxis, curveData, eqdsk);  
  return intersect;
}

bool ClosedFluxCurve::fieldFollowingCase(Point& startPoint, Point& nextPoint)
{
  // Step 1: Set properties of m
  if (m == 0)
    m = 1;
  mDecreased = false;
  mIncreased = false;

  // Step 2: Until termination condition meet, keep finding next points.
  randomFactor = 1;
  while (true)
  {
    distanceSet = pMetaData.getNodeSpacingAtFlux(psiNorm);
    // Step 2.1: If randomStart, adjust the factor for random start
    if (eqdsk.randomStart() && tagStartingPoint &&!randomGen)
    {
      randomFactor = double(rand_r(&seed)%100000 + 1)/100000.0;
      randomGen = true;
    }

    // Step 2.2: Update distance based on randomFactor (if its not 
    // randomStart, distance just multiples by 1 so stays same.
    distanceSet = distanceSet*randomFactor;
    distance = distanceSet;

    curveData.hitOrigin = false;
    intersect = !findNextFieldFollowingPoint(startPoint, nextPoint, distance, m, curveData, eqdsk);
    distance = distance/distanceSet;
    if(distance < 1.0/(1.0 + eqdsk.getSpacingToleranceAbsolute()) && !intersect && !curveData.hitOrigin)
      updateM(m, false);
    else if(distance > (1.0 + eqdsk.getSpacingToleranceAbsolute()) && !intersect) // for now, allow wall hits to be too long
      updateM(m, true);
    else if(distance < 1.0/(1.0 + eqdsk.getSpacingToleranceOptimal()) && !intersect && !curveData.hitOrigin
            && mChanged && (!mDecreased || !mIncreased))
      updateM(m, false);
    else if(distance > (1.0 + eqdsk.getSpacingToleranceOptimal()) && !intersect
            && mChanged && (!mDecreased || !mIncreased))
      updateM(m,true);
    else
      break; // 1. distance acceptable, so use this point, or 2. meet boundary of what?(intersect)
  }
 
  return intersect;
}

void ClosedFluxCurve::updateM(int& m, bool increase)
{
  if (increase)
  {
    if(eqdsk.getIntraCurveSpacingSmallVariation())
      m = m+1;
    else
      m = m*2;

    mIncreased = true;
    mChanged = true;
  }
  else
  {
    if(eqdsk.getIntraCurveSpacingSmallVariation())
      m = m-1;
    else
      m = m/2;
    
    mDecreased = true;
    mChanged = true;
  }
}

Flux& ClosedFluxCurve::getFluxCurve()
{
  return f;
}

/***********************************************/
// Class: SeparatrixCurve
/***********************************************/
SeparatrixCurve::SeparatrixCurve(const PhysicsPoint& xPt, std::vector <Point> startPoints, EqdskData& eqdskData, const WallCurve& wallCurve, 
                                 const PlaneMetaData& planeMetaData):eqdsk(eqdskData), wall(wallCurve)
{
  psi = xPt.getPsi();
  xPoint = xPt.getPoint();
  psiNorm = eqdskData.convertPsiToNorm(xPt.getPsi());
  startPts = startPoints;

  curveData.psi = psi;
  curveData.xPoint = true;
  pMetaData = planeMetaData;
  curveData.origin = xPoint;
  std::vector <SeparatrixLeg> separatrixLegs;

  // Step 1: Get pushed points out of the x-points.
  double pushDistance = pMetaData.getNodeSpacingAtFlux(psiNorm);
  pushedPoints = getPushedPoints(xPt, pushDistance, eqdsk);

  // Step 2: Generate curve from pushed points
  for (int i = 0; i < pushedPoints.size(); i++)
  {
    SeparatrixLeg leg;
    leg.fieldPoints.push_back(xPoint);
    leg.numXPts++;
    leg.xPtAtStart = true;
    
    // Check if the pushed point is inside or outside the wallcurve
    // If outside, the distance between the Xpt and wall is less than
    // intra-curve spacing. The curve will have two points. One Xpt and 
    // second a point from the starting points container.
    if (!windingNumberPolygonTest(pushedPoints[i], wall.getPoints()))
    {
      std::cout << "The pushed point " << pushedPoints[i].x << " , " << pushedPoints[i].y << " is outside\n";
      std::cout << "the wall curve - Readjusted it on the wall\n"; 
      int intersectIndex = -1;
      intersectIndex = findStartPointIndexAtIntersection(xPoint, pushedPoints[i], startPts);
      assert (intersectIndex != -1);
      leg.fieldPoints.push_back(startPts[intersectIndex]);	
    }
    else
      getSeparatrixLeg(pushedPoints[i], leg);

    if (leg.fieldPoints.size())
      separatrixLegs.push_back(leg);
  }

  // Step 3: Generate curve from starting points
  for (int i = 0; i < startPts.size(); i++)
  {
    SeparatrixLeg leg;
    getSeparatrixLeg(startPts[i], leg);
    if (leg.fieldPoints.size())
      separatrixLegs.push_back(leg);
  }
  
  sepCurves =  mergeSeparatrixLegs(separatrixLegs);
}

void SeparatrixCurve::getSeparatrixLeg(const Point& point, SeparatrixLeg& leg)
{
  leg.fieldPoints.push_back(point);
  Point currentPoint = point;

  while (true)
  {
    intersect = false;
    curveData.hitOrigin = false;

    // Step 1: Set properties of m.
    if (m == 0)
      m = 1;
    mDecreased = false;
    mIncreased = false;

    // Step 2: Until termination condition meet, keep finding next points.
    Point nextPoint;
    while(true)
    {
      distance = pMetaData.getNodeSpacingAtFlux(psiNorm);
      intersect = !findNextFieldFollowingPoint(currentPoint, nextPoint, distance, m, curveData, eqdsk);
      distance = distance/pMetaData.getNodeSpacingAtFlux(psiNorm);
      if(distance < 1.0/(1.0 + eqdsk.getSpacingToleranceAbsolute()) && !intersect && !curveData.hitOrigin)
        updateM(m, false);
      else if(distance > (1.0 + eqdsk.getSpacingToleranceAbsolute()) && !intersect) // for now, allow wall hits to be too long
        updateM(m, true);
      else if(distance < 1.0/(1.0 + eqdsk.getSpacingToleranceOptimal()) && !intersect && !curveData.hitOrigin
            && mChanged && (!mDecreased || !mIncreased))
        updateM(m, false);
      else if(distance > (1.0 + eqdsk.getSpacingToleranceOptimal()) && !intersect
            && mChanged && (!mDecreased || !mIncreased))
        updateM(m,true);
      else
        break;  // distance acceptable, use this point  
    }  // end of Inner WHILE loop
 
    mChanged = false;
    bool pointOutside = false;
    int numIntersections = getNumIntersection(currentPoint, nextPoint, wall);
    int intersectIndex = -1;
    int numMinPoints = (leg.fieldPoints.size() == 1) ? 1:0;
    if (numIntersections > numMinPoints)
    {
      intersectIndex = findStartPointIndexAtIntersection(currentPoint, nextPoint, startPts);
      assert (intersectIndex != -1);
      leg.fieldPoints.push_back(startPts[intersectIndex]);
      pointOutside = true;
    }
    else if (!windingNumberPolygonTest(nextPoint, wall.getPoints()))
    {
      leg.fieldPoints.clear();
      pointOutside = true;
    }
    if (pointOutside)
      return;

    if (intersect)
    {
      assert(!curveData.hitOrigin);
      leg.fieldPoints.push_back(nextPoint);
      return; // end this curve generation. -- Intersect
    } 
    else if (curveData.hitOrigin)
    {
      double distPrev = pMetaData.getNodeSpacingAtFlux(psiNorm);
      if (distance2D(nextPoint, xPoint) > distPrev*0.5)
        leg.fieldPoints.push_back(nextPoint);

      leg.fieldPoints.push_back(xPoint);
      leg.numXPts++;
      leg.xPtAtEnd = true;
      return;
    } 
    else
    {
      leg.fieldPoints.push_back(nextPoint);
      currentPoint = nextPoint;  
    }  
  }
}

void SeparatrixCurve::updateM(int& m, bool increase)
{
  if (increase)
  {
    m = m+1;
    mIncreased = true;
    mChanged = true;
  }
  else
  {
    m = m-1;
    mDecreased = true;
    mChanged = true;
  }
}

// Stitch all separatrix legs together for a curve.
std::vector <Flux> SeparatrixCurve::mergeSeparatrixLegs(std::vector <SeparatrixLeg>& separatrixLegs)
{
  std::vector <Flux> fluxCurves; // size = 1 or 2 depending on type of separatrix
  std::vector <int> segFront, segBack;
  for (int i = 0; i < separatrixLegs.size(); i++)
  {
    SeparatrixLeg& leg = separatrixLegs[i];
    if (leg.numXPts == 1)
    {
      if (leg.xPtAtStart == true)
        segBack.push_back(i);
      else if (leg.xPtAtEnd == true)
        segFront.push_back(i);
      
      leg.curveSubType = CurveSubType::Open;
    }
    else if (leg.numXPts == 2)
      leg.curveSubType = CurveSubType::Closed;
    else 
      continue;
  }

  assert (segFront.size() == segBack.size());
  assert (segFront.size() <= 2);

  if (segFront.size() == 2)
  { // separatrix curve with 4 legs
    int indexVert0 = separatrixLegs[segFront[0]].fieldPoints.size() - 2;  // -1 is xPt itself
    int indexVert1 = 1;  // second member of the curve (0 = first is xPt itself).
    int indexVert2 = separatrixLegs[segFront[1]].fieldPoints.size() - 2;
    int indexVert3 = 1;
    Point vert0 = separatrixLegs[segFront[0]].fieldPoints[indexVert0];
    Point vert1 = separatrixLegs[segBack[0]].fieldPoints[indexVert1];
    Point vert2 = separatrixLegs[segFront[1]].fieldPoints[indexVert2];
    Point vert3 = separatrixLegs[segBack[1]].fieldPoints[indexVert3];

    Point intersectPt1, intersectPt2;
    double sineTolerance = 0.001;
    int numIntersection = intersectBetweenTwoLineSegments(vert0, vert1, vert2, vert3, 0.0, sineTolerance, intersectPt1, intersectPt2);
    if (numIntersection)
    {
      int iTemp = segBack[0];
      segBack[0] = segBack[1];
      segBack[1] = iTemp;
    }

    for (int i = 0; i < segFront.size(); i++)
    {
      Flux flux;
      SeparatrixLeg startLeg = separatrixLegs[segFront[i]];
      SeparatrixLeg endLeg = separatrixLegs[segBack[i]];
      flux.separatrixLegs.push_back(startLeg);
      flux.separatrixLegs.push_back(endLeg);
      flux.fieldPoints.assign(startLeg.fieldPoints.begin(), startLeg.fieldPoints.end() - 1);
      flux.fieldPoints.assign(endLeg.fieldPoints.begin(), endLeg.fieldPoints.end());
      flux.curveType = CurveType::Separatrix;
      PhysicsPoint pt(xPoint, psi, PointType::XPoint);
      flux.xPoint = pt;
      flux.psiNormOnFlux = psiNorm;
      fluxCurves.push_back(flux);
    }
  } 
  else if (segFront.size() == 1)
  { // separatrix curve with 2 legs and 1 closed loop
    Flux flux;
    SeparatrixLeg startLeg = separatrixLegs[segFront[0]];
    flux.separatrixLegs.push_back(startLeg);
    flux.fieldPoints.assign(startLeg.fieldPoints.begin(), startLeg.fieldPoints.end() - 1);
    for (int i = 0; i < separatrixLegs.size(); i++)
    {
      if (separatrixLegs[i].curveSubType == CurveSubType::Closed)
      {
        SeparatrixLeg closed = separatrixLegs[i];
        flux.separatrixLegs.push_back(separatrixLegs[i]);
        flux.fieldPoints.assign(closed.fieldPoints.begin(), closed.fieldPoints.end()); 
      }
    }
    SeparatrixLeg endLeg = separatrixLegs[segBack[0]];
    flux.separatrixLegs.push_back(endLeg);
    flux.fieldPoints.assign(endLeg.fieldPoints.begin() + 1, endLeg.fieldPoints.end());
    flux.curveType = CurveType::Separatrix;
    PhysicsPoint pt(xPoint, psi, PointType::XPoint);
    flux.xPoint = pt;
    flux.psiNormOnFlux = psiNorm;
    fluxCurves.push_back(flux);    
  }
  
  return fluxCurves;
}

const std::vector <Flux>& SeparatrixCurve::getFluxCurves() const
{
  return sepCurves;
}

/***********************************************/
// Class: OpenCurve
/***********************************************/
OpenFluxCurve::OpenFluxCurve(std::vector <PhysicsPoint> startPoints, int startPtIndex, EqdskData& eqdskData, const PhysicsPoint& oPoint,
                             const WallCurve& wallCurve, const PlaneMetaData& planeMetaData): eqdsk(eqdskData)
{
  // Step 1: Set general class variables
  wall = wallCurve;
  pMetaData = planeMetaData;
  magneticAxis = oPoint; 
 
  for (int i = 0; i < startPoints.size(); i++)
    startPts.push_back(startPoints[i].getPoint());
  
  // Step 2: Set specific flux curve varibales
  index = startPtIndex;
  Point startPoint = startPts[index];
  psi = startPoints[startPtIndex].getPsi();  // All starting points have same psi value.
  psiNorm = eqdskData.convertPsiToNorm(psi);
  f.psiNormOnFlux = psiNorm;
  f.fieldPoints.push_back(startPoint);

  // Step 3: Set curve meta data.
  curveData.psi = psi;
  curveData.origin = startPoint;

  // Step 4: Start Tracing next points from starting point
  Point nextPoint = startPoint;
  while (true)
  {
    if (eqdsk.getIntraCurveSpacingOption() == -2)
      intersect = nonFieldFollowingCase(startPoint, nextPoint);
    else 
      intersect = fieldFollowingCase(startPoint, nextPoint);
    
    assert(!intersect); 
    mChanged = false;
    int numIntersections = getNumIntersection(startPoint, nextPoint, wall);
    int intersectIndex = -1;
    success = false;
    if (f.fieldPoints.size() == 1)
    {
      intersectIndex = findStartPointIndexAtIntersection(startPoint, nextPoint, startPts, index);
      if (intersectIndex == -1)
      {
        if (!windingNumberPolygonTest(nextPoint, wall.getPoints()))
          return;
      }
      else
      {
        Point midPoint(0.5*(startPoint.x + startPts[intersectIndex].x), 0.5*(startPoint.y + startPts[intersectIndex].y));
        if (!windingNumberPolygonTest(midPoint, wall.getPoints()))
          return;
        else
        {
          f.fieldPoints.push_back(startPts[intersectIndex]);
          success = true;
          true;
        }
      }
    }
    else if (numIntersections)
    {
      intersectIndex = findStartPointIndexAtIntersection(startPoint, nextPoint, startPts);
      assert(intersectIndex != -1);
      f.fieldPoints.push_back(startPts[intersectIndex]);
      success = true;
      return;
    }
    
    f.fieldPoints.push_back(nextPoint);
    if (curveData.hitOrigin)
      assert(0);
    startPoint = nextPoint;
  }
}

bool OpenFluxCurve::nonFieldFollowingCase(Point& startPoint, Point& nextPoint)
{
  distance = pMetaData.getNodeSpacingAtFlux(psiNorm);  
  curveData.hitOrigin = false;

  intersect = !findNextPoint(startPoint, nextPoint, distance, magneticAxis, curveData, eqdsk);  
  return intersect;
}

bool OpenFluxCurve::fieldFollowingCase(Point& startPoint, Point& nextPoint)
{
  // Step 1: Set properties of m
  if (m == 0)
    m = 1;
  mDecreased = false;
  mIncreased = false;

  // Step 2: Until termination condition meet, keep finding next points.
  while (true)
  {
    distanceSet = pMetaData.getNodeSpacingAtFlux(psiNorm);
    distance = distanceSet;
    curveData.hitOrigin = false;

    intersect = !findNextFieldFollowingPoint(startPoint, nextPoint, distance, m, curveData, eqdsk);
    distance = distance/distanceSet;

    if(distance < 1.0/(1.0 + eqdsk.getSpacingToleranceAbsolute()) && !intersect && !curveData.hitOrigin)
      updateM(m, false);
    else if(distance > (1.0 + eqdsk.getSpacingToleranceAbsolute()) && !intersect) // for now, allow wall hits to be too long
      updateM(m, true);
    else if(distance < 1.0/(1.0 + eqdsk.getSpacingToleranceOptimal()) && !intersect && !curveData.hitOrigin
            && mChanged && (!mDecreased || !mIncreased))
      updateM(m, false);
    else if(distance > (1.0 + eqdsk.getSpacingToleranceOptimal()) && !intersect
            && mChanged && (!mDecreased || !mIncreased))
      updateM(m,true);
    else
      break; // 1. distance acceptable, so use this point, or 2. meet boundary of what?(intersect)
  }
 
  return intersect;
}

void OpenFluxCurve::updateM(int& m, bool increase)
{
  if (increase)
  {
    if(eqdsk.getIntraCurveSpacingSmallVariation())
      m = m+1;
    else
      m = m*2;

    mIncreased = true;
    mChanged = true;
  }
  else
  {
    if(eqdsk.getIntraCurveSpacingSmallVariation())
      m = m-1;
    else
      m = m/2;
    
    mDecreased = true;
    mChanged = true;
  }
}

const Flux& OpenFluxCurve::getFluxCurve() const
{
  return f;
}

const bool& OpenFluxCurve::useStartPoint() const
{
  return success;
}

/***********************************************/
// Other Helping Functions
/***********************************************/
std::vector <Flux> getOpenCurvesOnFace(pGFace gf, std::vector <double> psiValues, EqdskData& eqdskData, const WallCurve& wall,
                                       const PhysicsPoint& oPoint, const PlaneMetaData& planeMetaData)
{
  std::vector <Flux> openCurves;
  std::vector <std::vector <PhysicsPoint>> startPoints = getStartPointsOnSimFace(gf, psiValues, eqdskData);
//#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < startPoints.size(); i++)
  {
    std::vector <PhysicsPoint> startPointsForPsi = startPoints[i];
    for (int j = 0; j < startPointsForPsi.size(); j++)
    {
      double psi = startPointsForPsi[j].getPsi();
      if (fabs(eqdskData.convertPsiToNorm(psi) - 1.45) < 1e-5){
        std::cout << "=================================\n";
        std::cout << " Start Point To Use = " << startPointsForPsi[j].getPoint().x << " , " << startPointsForPsi[j].getPoint().y << "\n";
      }
      OpenFluxCurve openFluxCurve(startPointsForPsi, j, eqdskData, oPoint, wall, planeMetaData);
      if (fabs(eqdskData.convertPsiToNorm(psi)- 1.45) < 1e-5)
        std::cout << "Use Point = " << openFluxCurve.useStartPoint() << "\n";
      if(!openFluxCurve.useStartPoint())
        continue;
      Flux f = openFluxCurve.getFluxCurve();
      // if (!validOpenCurve() add later

      restrictDistanceOfLastEdge(f, eqdskData);
//#pragma omp critical
      {
        openCurves.push_back(f);
      }
    }
  }

  return openCurves;
}

std::vector<double> getSpacingOnPrivateRegion(pGFace gf, const PlaneMetaData& planeMetaData)
{
  // if pvtSpacing on specific face given use it. 
  // else
  std::vector <double> spacing = planeMetaData.getPlaneFluxValues();
  
  return spacing;
}
