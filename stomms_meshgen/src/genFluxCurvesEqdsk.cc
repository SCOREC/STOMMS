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
    fluxCurve.curveType = CurveType::closed;
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
