#include "modelingEqdsk.h"

Model generateModelEqdsk()
{

}

pGModel generateSimModel()
{
  // Step 1: Create a new Simmetrix model (pGModel)
  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);
  pGModel model = GM_new(0);

  // createModelVertices
  // createModelEdge { call function to create model curves in this }
  // create ModelFace{ write function for concentric faces and separatrix curve faces or one more generic function).

  Progress_delete(prog);
  return model;
}



