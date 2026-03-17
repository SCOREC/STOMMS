#include "plane.h"

/***********************************************/
// Class: Plane
/***********************************************/
void Plane::setFieldPointsOnFlux(const std::vector <FluxParametricPoints>& fieldPoints)
{
  fieldPointsOnFluxCurves = fieldPoints;
}

const std::vector <FluxParametricPoints>& Plane::getFieldPointsOnFluxCurves() const
{
  return fieldPointsOnFluxCurves;
}
