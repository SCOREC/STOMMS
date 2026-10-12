#ifndef MODELINGEQDSK_h
#define MODELINGEQDSK_h

#include "modeling.h"
#include "magneticGeometry.h"
#include "genFluxCurvesEqdsk.h"
#include "physicsAttributes.h"

pGModel generateSimModel(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData);

// Classification Function
void classifyModelFaces(pGModel model);

#endif
