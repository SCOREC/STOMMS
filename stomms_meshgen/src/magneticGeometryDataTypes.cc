#include "magneticGeometryDataTypes.h"

/***********************************************/
// Class: Flux
/***********************************************/
void Flux::setSimEdgeToFlux(pGEdge ge)
{
  Edge modelEdge;
  modelEdge.setSimEdge(ge);
  edgesOnFlux.push_back(modelEdge);
}

/***********************************************/
// Class: CurveContainer
/***********************************************/
CurveContainer::CurveContainer(std::vector <Flux>& closedCurves, std::vector <Flux>& separatrices, WallCurve& wall)
{
  // Step 1: Set curves
  curvesClosed = closedCurves;
  curvesSeparatrix = separatrices;
  wallCurve = wall;
}

void CurveContainer::setCriticalPoints(const std::vector <PhysicsPoint>& oPts, const std::vector <PhysicsPoint>& xPts)
{
  oPoints = oPts;
  xPoints = xPts;
}

void CurveContainer::setOpenCurves(const std::vector <Flux>& openFluxCurves)
{
  openCurves = openFluxCurves;
}

std::vector <Flux>& CurveContainer::getCurvesClosed()
{
  return curvesClosed;
}

std::vector <Flux>& CurveContainer::getCurvesSeparatrix() 
{
  return curvesSeparatrix;
}

std::vector <Flux>& CurveContainer::getCurvesOpen()
{
  return openCurves;
}

WallCurve& CurveContainer::getWallCurve()
{
  return wallCurve;
}

const std::vector <PhysicsPoint>& CurveContainer::getOPoints() const
{
  return oPoints;
}

const std::vector <PhysicsPoint>& CurveContainer::getXPoints() const
{
  return xPoints;
}
