#ifndef PLANE_H
#define PLANE_H 

#include "fieldPointsOnFlux.h"
/**
 * A class to define geometric model on a plane.
 */
class Plane{
  public:
    std::vector <Face> modelFaces;
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    Vertex oPoint;
    int planeNumber;  // plane number starting from 0 to numPlanes-1

    /**
     * Function to set field following points on all the flux curves of the plane.
     * @param fieldPoints: a vector of the class that holds field following points info.
     */ 
    void setFieldPointsOnFlux(const std::vector <FluxParametricPoints>& fieldPoints);

    /**
     * Function to get a vector of class that holds field following points info.
     * @return a vector of class FluxParametricPoints.
     */   
    const std::vector <FluxParametricPoints>& getFieldPointsOnFluxCurves() const;
  private:
    std::vector <FluxParametricPoints> fieldPointsOnFluxCurves;
};

#endif
