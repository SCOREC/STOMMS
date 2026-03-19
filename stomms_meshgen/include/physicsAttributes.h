#ifndef PHYSICSATTRIBUTES_H
#define PHYSICSATTRIBUTES_H

#include "modelTopology.h"
#include "criticalPoints.h"
#include "magneticGeometryDataTypes.h"

// Model Face functions
bool isModelFaceOnCore(pGFace gf);
int getNumPeriodicEdgesOnModelFace(pGFace gf);
std::vector <pGVertex> getCriticalPointsOnModelFace(pGFace gf, PointType pointType);
void tagModelFacesAdjacentToXPoint(pGVertex gv, int index);
void setFaceTagsOnInnerSeparatrix(pGVertex gv, int index);
void setFaceTagsOnOuterSeparatrix(pGVertex gv, int index);
bool isFaceBoundedByTwoFluxCurves(pGFace gf);

// Model Vertex Functions
bool compareVertexPsi(pGVertex gv1, pGVertex gv2);

// Model functions
std::vector <pGVertex> getCriticalPointsOnModel(pGModel model, PointType pointType);

#endif
