#ifndef PHYSICSATTRIBUTES_H
#define PHYSICSATTRIBUTES_H

#include "modelTopology.h"
#include "criticalPoints.h"
#include "magneticGeometryDataTypes.h"

// Model Face functions

/**
 * Function to check if a model face is on the core region or not.
 * @param gf: model face.
 * @return true if model face is on the core, false otherwise.
 */
bool isModelFaceOnCore(pGFace gf);

/**
 * Function to get number of periodic model edges bounding the model face.
 * @param gf: model face.
 * @return number of periodic edges bounding the face gf.
 */
int getNumPeriodicEdgesOnModelFace(pGFace gf);

/**
 * Function to get a vector of model vertices classified on the model face.
 * @param gf: model face.
 * @param pointType: type of critical point (OPoint, XPoint).
 * @return a vector of model vertices of type pointType. Empty vector if finds no such point.
 */
std::vector <pGVertex> getCriticalPointsOnModelFace(pGFace gf, PhysicsPointType pointType);

/**
 * Function to set classification on model faces adjacent to X-point.
 * Its top level function. Next two functions are implementation functions.
 * @param gv: x-point model vertex.
 * @param index: index of the X-point (to figure out if its primary x-point or not). 
 */
void tagModelFacesAdjacentToXPoint(pGVertex gv, int index);

/**
 * Function to set classification on model faces adjacent to Inner X-point (primary).
 * @param gv: x-point model vertex.
 * @param index: index of the X-point (to figure out if its primary x-point or not). 
 */
void setFaceTagsOnInnerSeparatrix(pGVertex gv, int index);

/**
 * Function to set classification on model faces adjacent to outer separatrix.
 * @param gv: x-point model vertex.
 * @param index: index of the X-point (to figure out if its primary x-point or not). 
 */
void setFaceTagsOnOuterSeparatrix(pGVertex gv, int index);

/**
 * Function to check if a model face is bounded by edges with two different psi values.
 * @param gf: model face.
 * @return true if model face is bounded by edges with two different psi values, false otherwise.
 */
bool isFaceBoundedByTwoFluxCurves(pGFace gf);

// Model Vertex Functions
/**
 * Function to compare psi values of two model vertices.
 * @param gv1: model vertex 1.
 * @param gv2: model vertex 2.
 * @return true if psi value of 1 is less or equal to psi value of 2.
 */
bool compareVertexPsi(pGVertex gv1, pGVertex gv2);

// Model functions
/**
 * Functon to return model vertices of critical points on the model.
 * @param model: simmetrix model.
 * @param pointType: type of critical point (OPoint, XPoint).
 * @return a vector of model vertices of type pointType.
 */
std::vector <pGVertex> getCriticalPointsOnModel(pGModel model, PhysicsPointType pointType);

#endif
