#ifndef FIELDPOINTSONFLUX_H
#define FIELDPOINTSONFLUX_H

#include "magneticGeometryDataTypes.h"

/**
 * A class to hold flux parametric points properties.
 */
class FluxParametricPoints{
  public:
    FluxParametricPoints(){};
    FluxParametricPoints(const Flux& flux, int pointPlacementType);
    FluxParametricPoints(const Flux& flux, std::vector <std::vector<double>> parValuesOnEdges);
    const std::vector <Edge>& getFluxEdges() const;
    const std::vector <std::vector<double>>& getParametricValuesOnFlux() const;
    const std::vector<double>& getParametricValuesAtFluxEdge(const Edge& ge) const;
  private:
    int placementType = 0; // 0 = spacing between nodes, 1 = field points
    Flux f;
    std::vector <Edge> fluxEdges;
    std::vector <std::vector<double>> parametricValues;
};

// Functions to set the vertices parameters on flux curve.
std::vector <std::vector<double>> setVerticesParValuesOnFlux(Flux f, int type);
std::vector <std::vector<double>> setVerticesParValuesUsingDistance(Flux f);
std::vector <std::vector<double>> setParOnClosedFluxUsingDistance(Flux f);

double getNextParamPointForDist(const Edge& ge, double parStart, double parEnd, double targetLength);

#endif
