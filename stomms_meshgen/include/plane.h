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
    
    /**
     * Function to set model vertices and edges from the set of model faces.
     * Uses downward adjacencies functions from Simmetrix.
     * @param modelFaces: a vector of model faces on the poloidal plane.
     */ 
    void setModelEntitiesFromModelFaces(const std::vector <Face>& modelFaces);

    /**
     * Function to set x-point (push back to xpoint vector in the class).
     * @param gv: Simmetrix mesh vertex.
     */ 
    void setXPoint(const pGVertex& gv);

    /**
     * Function to set x-point (push back to xpoint vector in the class).
     * @param v: STOMMS local mesh vertex.
     */ 
    void setXPoint(const Vertex& v);

    /**
     * Function to get a vector of class that holds field following points info.
     * @return a vector of class FluxParametricPoints.
     */   
    const std::vector <FluxParametricPoints>& getFieldPointsOnFluxCurves() const;

    /**
     * Function to get a vector of model edges on the plane.
     * @return a vector of model edges on the plane
     */ 
    const std::vector <Edge>& getModelEdgesOnPlane() const;

    /**
     * Function to get a vector of model vertices on the plane.
     * @return a vector of model vertices on the plane
     */ 
    const std::vector <Vertex>& getModelVerticesOnPlane() const;

    /**
     * Function to get a vector of model vertices classified as xpoints. 
     * @return a vector of model vertices on the plane classified as xpoints. 
     */ 
    const std::vector <Vertex>& getXPointsOnPlane() const;
  private:
    std::vector <FluxParametricPoints> fieldPointsOnFluxCurves;
    std::vector <Edge> modelEdges;
    std::vector <Vertex> modelVertices;
    std::vector <Vertex> xPoints;
};

#endif
