#ifndef PLANE_H
#define PLANE_H 

#include "fieldPointsOnFlux.h"
/**
 *
 * A class to define geometric model on a plane.
 */
class Plane{
  public:
    std::vector <Face> modelFaces;
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    Vertex oPoint;
    int planeNumber;  // plane number starting from 0 to numPlanes-1

    // Set Functions
    void setFieldPointsOnFlux(const std::vector <FluxParametricPoints>& fieldPoints);

    // Get Functions
    const std::vector <FluxParametricPoints>& getFieldPointsOnFluxCurves() const;
  private:
    std::vector <FluxParametricPoints> fieldPointsOnFluxCurves;
};

#endif
