#include "genFluxCurvesEqdsk.h"

/**
 * in modelingEqdsk.cc 
 * Model genSimModel(const EqdskData& eqdsk, ....any other info)
 * .. genModelFaces
 * .. genModelVertices
 * .. genModelCurves
 * .. genModelEdges ...
 */

/**
 * in modelDataEqdsk()
 * Set Basic properties of whatever ...
 */

/**
 * Top Level function
 * genFluxCurves(const EqdskData& eqdsk)
 * {
 *   genFluxClosed();
 *   genFluxSeparatrix();
 *   genFluxOpen();
 *     genFluxInPrivateRegion ..???
 *
 * }
 */


/***********************************************/
// Class ClosedCurve
/***********************************************/

std::vector <Flux> genClosedFluxCurves(const std::vector <double>& corePsiValues, EqdskData& eqdskData)
{
  std::vector <PhysicsPoint> startPoints = getStartPointClosed(corePsiValues, eqdskData);
  std::vector <Flux> closedFluxCurves;
  for (int i = 0; i < startPoints.size(); i++)
  {
    unsigned int mySeed = 1024 + i +1; // for random start
    PhysicsPoint startPoint = startPoints[i];
    ClosedFluxCurve closedFluxCurve(startPoint, mySeed, eqdskData);
    Flux fluxCurve = closedFluxCurve.getFluxCurve();
    fluxCurve.curveType = CurveType::Closed;
  }

  return closedFluxCurves;
}

ClosedFluxCurve::ClosedFluxCurve(const PhysicsPoint& startPt, unsigned int& mySeed, EqdskData& eqdskData):
                                 seed(mySeed), eqdsk(eqdskData)
{
  DomainBox box = eqdskData.getDomainBox();
  Point startPoint = startPt.getPoint();
  psiNorm = eqdskData.convertPsiToNorm(startPt.getPsi());
  f.fieldPoints.push_back(startPoint);
  CurveMetaData curveData;
  curveData.psi = psiNorm;
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
  distanceSet = eqdsk.getFluxInputData().fluxMeshSpacing.at(psiNorm);
  
  if (eqdsk.randomStart() && !randomGen)
  {
    randomFactor = double(rand_r(&seed)%100000 + 1)/100000;
    randomGen = true;
  }

  distanceSet = distanceSet*randomFactor;
  distance = distanceSet;

  curveData.hitOrigin = false;
  PhysicsPoint oPoint; // MAKE THIS OPOINT AVAILABLE HERE.
  intersect = findNextPoint(startPoint, nextPoint, distance, oPoint, curveData, eqdsk);  
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
  distanceSet = eqdsk.getFluxInputData().fluxMeshSpacing.at(psiNorm);
  while (true)
  {
    // Step 2.1: If randomStart, adjust the factor for random start
    if (eqdsk.randomStart() && !randomGen)
    {
      randomFactor = double(rand_r(&seed)%100000 + 1)/100000;
      randomGen = true;
    }

    // Step 2.2: Update distance based on randomFactor (if its not 
    // randomStart, distance just multiples by 1 so stays same.
    distanceSet = distanceSet*randomFactor;
    distance = distanceSet;

    curveData.hitOrigin = false;
    intersect = findNextFieldFollowingPoint(startPoint, nextPoint, distance, m, curveData, eqdsk);
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

std::vector <Flux> genSeparatrixCurves(const std::vector <PhysicsPoint>& xPts, EqdskData& eqdskData, const WallCurve& wall)
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
      SeparatrixCurve separatrixCurve(xPts[i], startPoints, eqdskData, wall);
      std::vector <Flux> sepCurvesFromXpt = separatrixCurve.getFluxCurves();
      separatrices.insert(separatrices.end(), sepCurvesFromXpt.begin(), sepCurvesFromXpt.end());
    }
  }  
  return separatrices;
}

SeparatrixCurve::SeparatrixCurve(const PhysicsPoint& xPt, std::vector <Point> startPoints, EqdskData& eqdskData, const WallCurve& wallCurve):
                                 eqdsk(eqdskData), wall(wallCurve)
{
  psi = xPt.getPsi();
  xPoint = xPt.getPoint();
  psiNorm = eqdskData.convertPsiToNorm(xPt.getPsi());
  startPts = startPoints;

  CurveMetaData curveData;
  curveData.psi = psiNorm;
  curveData.xPoint = true;
  

  std::vector <SeparatrixLeg> separatrixLegs;
  // Step 1: Get pushed points out of the x-points.
  double pushDistance = eqdsk.getFluxInputData().fluxMeshSpacing.at(psiNorm);
  pushedPoints = getPushedPoints(xPt, pushDistance, eqdsk);

  // Step 2: Generate curve from pushed points
  for (int i = 0; i < pushedPoints.size(); i++)
  {
    SeparatrixLeg leg;
    leg.fieldPoints.push_back(xPoint);
    leg.numXPts++;
    leg.xPtAtStart = true;
    getSeparatrixLeg(pushedPoints[i], leg);
    if (leg.fieldPoints.size())
      separatrixLegs.push_back(leg);
  }

  // Step 3: Generate curve from starting points
  for (int i = 0; i < startPts.size(); i++)
  {
    SeparatrixLeg leg;
    leg.fieldPoints.push_back(startPts[i]);
    getSeparatrixLeg(startPts[i], leg);
    if (leg.fieldPoints.size())
      separatrixLegs.push_back(leg);
  }
  
  sepCurves =  mergeSeparatrixLegs(separatrixLegs);
}


// 
void SeparatrixCurve::getSeparatrixLeg(const Point& point, SeparatrixLeg& leg)
{
  leg.fieldPoints.push_back(point);
  Point currentPoint = point;

  distance = eqdsk.getFluxInputData().fluxMeshSpacing.at(psiNorm);
  while (true)
  {
    intersect = false;

    // Step 1: Set properties of m.
    if (m == 0)
      m = 1;
    mDecreased = false;
    mIncreased = false;

    // Step 2: Until termination condition meet, keep finding next points.
    Point nextPoint;
    while(true)
    {
      intersect = findNextFieldFollowingPoint(currentPoint, nextPoint, distance, m, curveData, eqdsk);
      distance = distance/eqdsk.getFluxInputData().fluxMeshSpacing.at(psiNorm);
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
    if (numIntersections > leg.fieldPoints.size())
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
    else if (!curveData.hitOrigin)
    {
      double distPrev = eqdsk.getFluxInputData().fluxMeshSpacing.at(psiNorm);
      if (distance2D(nextPoint, xPoint) < distPrev*0.5)
        leg.fieldPoints.push_back(nextPoint);

      leg.fieldPoints.push_back(xPoint);
      leg.numXPts++;
      leg.xPtAtEnd = true;
      return;
    } 
    else
      currentPoint = nextPoint;    
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
      if (leg.xPtAtStart = true)
        segBack.push_back(i);
      else if (leg.xPtAtEnd = true)
        segFront.push_back(i);
      
      leg.curveSubType = CurveSubType::Open;
    }
    else if (leg.numXPts == 2)
      leg.curveSubType = CurveSubType::Closed;
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
      int iBack = segBack[i];
      int iFront = segFront[i];
      flux.separatrixLegs.push_back(separatrixLegs[segFront[i]]);
      flux.separatrixLegs.push_back(separatrixLegs[segBack[i]]);
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
    flux.separatrixLegs.push_back(separatrixLegs[segFront[0]]);
    for (int i = 0; i < separatrixLegs.size(); i++)
    {
      if (separatrixLegs[i].curveSubType == CurveSubType::Closed)
        flux.separatrixLegs.push_back(separatrixLegs[i]); 
    }
    flux.separatrixLegs.push_back(separatrixLegs[segBack[0]]);
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
