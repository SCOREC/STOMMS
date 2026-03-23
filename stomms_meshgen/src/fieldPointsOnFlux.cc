#include "fieldPointsOnFlux.h"

/***********************************************/
// Class: FluxParametricPoints
// Finds the parametric values of field points
// on all the flux curves (on its edges). 
/***********************************************/
FluxParametricPoints::FluxParametricPoints(const Flux& flux, int pointPlacementType)
{
  // Step 1: Set the properties of the class.
  f = flux;
  fluxEdges = flux.edgesOnFlux;
  placementType = pointPlacementType;

  // Step 2: Iterate over the model edges and set parametric values
  parametricValues = setVerticesParValuesOnFlux(f, placementType);
}

// Constructor to set parametric points on flux based on fixed parametric values one edges. 
FluxParametricPoints::FluxParametricPoints(const Flux& flux, std::vector <std::vector<double>> parValuesOnEdges)
{
  // Step 1: Set the properties of the class.
  f = flux;
  fluxEdges = flux.edgesOnFlux;

  // Step 2: Iterate over the model edges and set parametric values
  parametricValues = parValuesOnEdges;
}

// Function to get the set of model edges on the flux curve.
const std::vector <Edge>& FluxParametricPoints::getFluxEdges() const
{
  return fluxEdges;
}

// Function to get the set of parametrix values on a flux curve.
const std::vector <std::vector<double>>& FluxParametricPoints::getParametricValuesOnFlux() const
{
  return parametricValues;
}

// Function to get the parametric value on a single edge.
const std::vector<double>& FluxParametricPoints::getParametricValuesAtFluxEdge(const Edge& ge) const
{
  int edgeIndex = -1;

  // Step 1: Get the simeEdge from the given Edge and compare its tag to the sim edges 
  // in the edges vector.
  pGEdge simEdge = ge.getSimEdge();
  for (int i = 0; i < fluxEdges.size(); i++)
  {
    pGEdge edge = fluxEdges[i].getSimEdge();
   
    // Step 2: if the tag is same, this is the edge we need.
    if (GEN_tag(edge) == GEN_tag(simEdge))
    {
      edgeIndex = i;
      break;
    }
  } 
  assert(edgeIndex >= 0);

  // Step 3: Return the vector of parametric values at the edge.
  return parametricValues[edgeIndex];
}

/***********************************************/
// Helper functions for setting/finding 
// parametrix values of points on flux curves.
/***********************************************/

// Top level function to set the parametric values on a flux curve.
std::vector <std::vector<double>> setVerticesParValuesOnFlux(Flux f, int type)
{
  std::vector <std::vector<double>> parValuesOnEdge;

  // Step 1: Based on the type of point placement type, find the parametric
  // values on the flux curve.
  if (type == 0)
    parValuesOnEdge = setVerticesParValuesUsingDistance(f);
  else if (type == 1)
    parValuesOnEdge = setVerticesParValuesUsingPoints(f);
  
  return parValuesOnEdge;
}

// Function to set parametric values on a flux based on a given fixed distance (spacing between points).
std::vector <std::vector<double>> setVerticesParValuesUsingDistance(Flux f)
{
  std::vector <std::vector<double>> parValuesOnEdge;

  // Step 1: Decide the type of flux curve and based on that find par values.
  if (f.curveType == CurveType::Closed)
    parValuesOnEdge = setParOnClosedFluxUsingDistance(f);
  // add open and separatrix cases later

  return parValuesOnEdge;
}

// Function to set parametric values on a flux based on a set of points(coordinates) already in the flux.
std::vector <std::vector<double>> setVerticesParValuesUsingPoints(Flux f)
{
  // Not dependent on flux type. One algorithm works for all curves type.
  std::vector <std::vector<double>> parValuesOnEdges;
  parValuesOnEdges = setParOnFluxUsingPoints(f);
  return parValuesOnEdges;
}

// Function to set up the field following points on the closed flux
// curve using the distance.
std::vector <std::vector<double>> setParOnClosedFluxUsingDistance(Flux f)
{
  std::vector <std::vector<double>> parametricValuesOnEdge;

  // Step 1: iterate over the model edges and set parametric values on each edge one
  // by one.
  for (int i = 0; i < f.edgesOnFlux.size(); i++)
  {
    std::vector <double> parValuesOnEdge;

    // Step 2: Get the model edge on flux curve and parameric bounds of the edge.
    Edge ge = f.edgesOnFlux[i];
    std::vector <double> parR = ge.getEdgeParRange();

    // Step 3: Push the first parametric value.
    parValuesOnEdge.push_back(parR[0]);

    // Step 4: Get the parametric values of the points satisfying nodeSpacing on flux.
    double nodeSpacing = f.nodeSpacingOnFlux;  // desired node spacing
    double parCurrent = parR[0];
    double parEnd = parR[1];
    while (ge.getEdgePartialLength(parCurrent, parEnd) > nodeSpacing)
    { // keep going until last found point is within the spacing of end point.
      double targetPar = getNextParamPointForDist(ge, parCurrent, parEnd, nodeSpacing);
      parValuesOnEdge.push_back(targetPar);
      parCurrent = targetPar;
    } 
    parametricValuesOnEdge.push_back(parValuesOnEdge);
  }

  return parametricValuesOnEdge;
}

// Gets the next parametric value on a flux curve based on starting point on the model edge and target length.
double getNextParamPointForDist(const Edge& ge, double parStart, double parEnd, double targetLength)
{
  // Step 1: Setup variables needed in the call.
  double lengthTolerance = 1e-5;
  int maxIter = 100;
  int numIter = 0;
  double low = parStart;
  double high = parEnd;
  double parTest;

  // Step 2: Use bisectioning method to narrow down the exact param value for the targetLength.
  while (numIter < maxIter)
  {
    // Step 2.1: Use bisectioning method (get the half way parameteric value).
    parTest = 0.5*(low + high);

    // Step 2.2: Get the length from start of curve to mid point.
    double length = ge.getEdgePartialLength(parStart, parTest);

    // Step 2.3: If close to targetLength, exit the loop, else adjust the parTest value.
    if (fabs(length - targetLength) < lengthTolerance)
      break;
    else if (length < targetLength)
      low = parTest;
    else
      high = parTest;

    numIter++;
  }

  return parTest;
}

// Function to set parametric values on a flux based on a set of points.
std::vector <std::vector<double>> setParOnFluxUsingPoints(Flux f)
{
  std::vector <std::vector<double>> parametricValuesOnEdge;

  int startPointIndex = 0;
  bool nextEdge = false;

  // Step 1: Iterate over the model edges and set par values on each edge one by one.
  for (int i = 0; i < f.edgesOnFlux.size(); i++)
  {
    std::vector <double> parValuesOnEdge;

    // Step 2: Get the model edge on flux curve and parameric bounds of the edge.
    Edge edge = f.edgesOnFlux[i];
    std::vector <double> parR = edge.getEdgeParRange();
    pGEdge ge = edge.getSimEdge();  // get sim edge
    bool periodicEdge = edge.edgeIsPeriodic();

    // Step 3: Always use first point (makes it easier to deal with periodic edges
    // using the algorithm in step 4.
    parValuesOnEdge.push_back(parR[0]);

    // Step 4: Iterate over the points on flux curves and save their parametric
    // value until they hit the edge end point.
    for (int j = startPointIndex+1; j < f.fieldPoints.size(); j++)
    {
      std::array <double, 3> pt = {f.fieldPoints[j].x, f.fieldPoints[j].y, 0.0};
      double par;
      GE_closestPoint(ge, pt.data(), nullptr, &par);
 
      // Step 4.1: Check if the par is same as par at the end point of the edge.
      // Both conditions are needed for periodic edge, where end point can have
      // par value equal to par0 or par1. 
      if (fabs(par - parR[1]) < 1e-8 || fabs(par - parR[0]) < 1e-8)
      {
        startPointIndex = j;
        nextEdge = true;
        break;    
      }
      parValuesOnEdge.push_back(par);
    }
    
    // Step 5: If edge is not periodic, push last point too. 
    if (!periodicEdge)
      parValuesOnEdge.push_back(parR[1]);

    // Step 6: Set the par values vector to main 2D vector. 
    parametricValuesOnEdge.push_back(parValuesOnEdge);

    // Step 7: If next edge, reset the nextEdge (we might not need it) 
    if (nextEdge)
      nextEdge = false;  // reset it and move to next edge
  }
  return parametricValuesOnEdge;
}

