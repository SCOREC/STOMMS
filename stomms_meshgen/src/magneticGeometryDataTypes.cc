#include "magneticGeometryDataTypes.h"

/***********************************************/
// Class: Flux
/***********************************************/

// To set a model edge to the flux curve.
void Flux::setSimEdgeToFlux(pGEdge ge)
{
  Edge modelEdge;
  modelEdge.setSimEdge(ge);
  edgesOnFlux.push_back(modelEdge);
}

/***********************************************/
// Class: CurveContainer
/***********************************************/

// Constructor to set flux curves already been generated. Takes in set of closed flux curves, 
// separatrices and wall curve.
CurveContainer::CurveContainer(std::vector <Flux>& closedCurves, std::vector <Flux>& separatrices, WallCurve& wall)
{
  // Step 1: Set curves
  curvesClosed = closedCurves;
  curvesSeparatrix = separatrices;
  wallCurve = wall;
}

// Function to set critical points in the container.
void CurveContainer::setCriticalPoints(const std::vector <PhysicsPoint>& oPts, const std::vector <PhysicsPoint>& xPts)
{
  oPoints = oPts;
  xPoints = xPts;
}

// Function to set open flux curves in the container.
void CurveContainer::setOpenCurves(const std::vector <Flux>& openFluxCurves)
{
  openCurves = openFluxCurves;
}

// Function to set wall curve edges in the container.
void CurveContainer::setWallEdges(const std::vector <Edge>& edgesOnWall)
{
  wallEdges = edgesOnWall;
}

// Function to get a vector of closed flux curves in the container. 
std::vector <Flux>& CurveContainer::getCurvesClosed()
{
  return curvesClosed;
}

// Function to get a vector of separatrix curves in the container.
std::vector <Flux>& CurveContainer::getCurvesSeparatrix() 
{
  return curvesSeparatrix;
}

// Function to get a vector of open curves in the container.
std::vector <Flux>& CurveContainer::getCurvesOpen()
{
  return openCurves;
}

// Function to get wall curve.
WallCurve& CurveContainer::getWallCurve()
{
  return wallCurve;
}

// Function to return vector of model edges on the wall.
const std::vector <Edge>& CurveContainer::getWallEdges()
{
  return wallEdges;
}

// Function to get a vector of O-points.
const std::vector <PhysicsPoint>& CurveContainer::getOPoints() const
{
  return oPoints;
}

// Function to get a vector of X-points.
const std::vector <PhysicsPoint>& CurveContainer::getXPoints() const
{
  return xPoints;
}
