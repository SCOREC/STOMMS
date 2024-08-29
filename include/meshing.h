#ifndef MESHING_H
#define MESHING_H

#include "model.h"

/*
 * Given the simmetrix model and planes data, this function generates
 * and return a simmetrix mesh.
 * pGModel model (in): The Simmetrix model created from modeling step.
 * std::vector <Plane> planes (in): The data for each plane  as object Plane is written to this container.
 * args* a (in): Input parameters.
 * returns a simmetrix mesh (pMesh).
*/
pMesh meshing(pGModel model, std::vector <Plane> planes, args* a);

#endif
