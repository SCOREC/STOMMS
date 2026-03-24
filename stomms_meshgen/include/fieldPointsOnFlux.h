#ifndef FIELDPOINTSONFLUX_H
#define FIELDPOINTSONFLUX_H

#include "magneticGeometryDataTypes.h"

/**
 * A class to hold flux parametric points properties.
 */
class FluxParametricPoints{
  public:
    /**
     * Default constructor.
     */ 
    FluxParametricPoints(){};

    /**
     * Constructor to set parametric points on flux based on the point placement type.
     * @param flux: input flux curve (contains all the information for points placement).
     * @param pointPlacementType: type of points placement. 0 = based on fixed distance (spcaing),
     * 				  = 1 based on already found field following points on flux.
     */ 
    FluxParametricPoints(const Flux& flux, int pointPlacementType);

    /**
     * Constructor to set parametric points on flux based on fixed parametric values one edges.
     * No calculation is done here. We just set data to the relative model edges. Needed in 
     * stellarator case where we have parametric values on plane 0, and we need to maintain
     * same parametric values on all the planes. 
     * @param flux: input flux curve (contains all the information for points placement).
     * @param parValuesOnEdges: parametric values on edges that need to be set on the corresponding
     *                          edges on different planes.
     */ 
    FluxParametricPoints(const Flux& flux, std::vector <std::vector<double>> parValuesOnEdges);

    /*
     * Function to get the set of model edges on the flux curve.
     * @return a vector of model edges.
     */ 
    const std::vector <Edge>& getFluxEdges() const;

    /**
     * Function to get the set of parametrix values on a flux curve.
     * @return a double vector where dimension 1 is equal to the number
     * of model edges.
     */ 
    const std::vector <std::vector<double>>& getParametricValuesOnFlux() const;

    /**
     * Function to get the parametric value on a single edge.
     * @return a vector of parametric values.
     */ 
    const std::vector<double>& getParametricValuesAtFluxEdge(const Edge& ge) const;
  private:
    int placementType = 0; // 0 = spacing between nodes, 1 = field points
    Flux f;  // input flux curve. contains all the info needed for parametric values calculations.
    std::vector <Edge> fluxEdges;  // vector of model edges on the flux.
    std::vector <std::vector<double>> parametricValues;  // calculated parametric values.
};

/** 
 * Top level function to set the parametric values on a flux curve.
 * @param flux: input flux curve (contains all the information for points placement).
 * @param pointPlacementType: type of points placement. 0 = based on fixed distance (spcaing),
 *                            = 1 based on already found field following points on flux.
 * @return a double vector where dimension 1 is equal to the number of model edges, and 
 * with a vector of corresponding parametric values.        
 */
std::vector <std::vector<double>> setVerticesParValuesOnFlux(Flux f, int type);

/**
 * Function to set parametric values on a flux based on a given fixed distance (spacing between points).
 * Used when pointPlacementType == 0.
 * @param flux: input flux curve. 
 * @return a double vector where dimension 1 is equal to the number of model edges, and 
 * with a vector of corresponding parametric values.        
 */
std::vector <std::vector<double>> setVerticesParValuesUsingDistance(Flux f);

/**
 * Function to set parametric values on a flux based on a set of points(coordinates) already in the flux.
 * Used when pointPlacementType == 1.
 * @param flux: input flux curve. 
 * @return a double vector where dimension 1 is equal to the number of model edges, and 
 * with a vector of corresponding parametric values.        
 */
std::vector <std::vector<double>> setVerticesParValuesUsingPoints(Flux f);

/**
 * Function to set parametric values on a closd flux based on a given fixed distance (spacing between points).
 * Low level function for the actual implementation. 
 * @param flux: input flux curve.
 * @return a double vector where dimension 1 is equal to the number of model edges, and
 * with a vector of corresponding parametric values.
 */
std::vector <std::vector<double>> setParOnClosedFluxUsingDistance(Flux f);

/**
 * Function to set parametric values on a flux based on a set of points.
 * Low level function for the actual implementation. 
 * @param flux: input flux curve.
 * @return a double vector where dimension 1 is equal to the number of model edges, and
 * with a vector of corresponding parametric values.
 */
std::vector <std::vector<double>> setParOnFluxUsingPoints(Flux f);

/**
 * Gets the next parametric value on a flux curve based on starting point on the model edge and target length.
 * @param ge: edge on which parametric value is being calculated.
 * @param parStart: starting point on the model edge. Need to walk targetLength along the edge from this point.
 * @param parEnd: parametric value of end point on the model edge. Need this to make sure we move towards right direction.
 * @param targetLength: distance to be moved.
 * @return parametric value of new point on the modele edge.
 */
double getNextParamPointForDist(const Edge& ge, double parStart, double parEnd, double targetLength);

#endif
