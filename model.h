#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "MeshSim.h"
#include "SimAdvModel.h"
#include "SimModel.h"
#include "SimUtil.h"
#include "input.h"


/*
 * From a given Vmec File, generate the model of the core region of stellarator 
*/
void generateCoreSimModel(pGModel &globalModel, args* a);

#endif
