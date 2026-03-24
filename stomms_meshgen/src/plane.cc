#include "plane.h"

/***********************************************/
// Class: Plane
/***********************************************/

// Function to set field following points on all the flux curves of the plane.
void Plane::setFieldPointsOnFlux(const std::vector <FluxParametricPoints>& fieldPoints)
{
  fieldPointsOnFluxCurves = fieldPoints;
}

// Function to get a vector of class that holds field following points info.
const std::vector <FluxParametricPoints>& Plane::getFieldPointsOnFluxCurves() const
{
  return fieldPointsOnFluxCurves;
}
