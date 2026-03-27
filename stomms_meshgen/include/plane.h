#ifndef PLANE_H
#define PLANE_H 

#include "fieldPointsOnFlux.h"
#include <set>

/**
 * A class to define geometric model on a plane.
 */
class Plane{
  public:
    std::vector <Face> modelFaces;
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    std::vector <Edge> wallEdges;
    Vertex oPoint;
    int planeNumber;  // plane number starting from 0 to numPlanes-1

    /**
     * Function to set field following points on all the flux curves of the plane.
     * @param fieldPoints: a vector of the class that holds field following points info.
     */ 
    void setFieldPointsOnFlux(const std::vector <FluxParametricPoints>& fieldPoints);

    void setModelEntitiesFromModelFaces(const std::vector <Face>& modelFaces);
    void setXPoint(const pGVertex& gv);
    void setXPoint(const Vertex& v);

    /**
     * Function to get a vector of class that holds field following points info.
     * @return a vector of class FluxParametricPoints.
     */   
    const std::vector <FluxParametricPoints>& getFieldPointsOnFluxCurves() const;

    const std::vector <Edge>& getModelEdgesOnPlane() const;
    const std::vector <Vertex>& getModelVerticesOnPlane() const;
    const std::vector <Vertex>& getXPointsOnPlane() const;
  private:
    std::vector <FluxParametricPoints> fieldPointsOnFluxCurves;
    std::vector <Edge> modelEdges;
    std::vector <Vertex> modelVertices;
    std::vector <Vertex> xPoints;
};

#endif
