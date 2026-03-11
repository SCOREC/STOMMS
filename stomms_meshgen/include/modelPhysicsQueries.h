#ifndef MODELPHYSICSQUERIES_H
#define MODELPHYSICSQUERIES_H

#include "modelTopology.h"
#include "magneticGeometryDataTypes.h"

std::vector <pGFace> getPrivateModelFaces(pGModel model);
std::vector <pGFace> getSOLModelFaces(pGModel model);

#endif
