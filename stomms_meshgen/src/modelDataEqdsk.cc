#include "modelDataEqdsk.h"

ModelEqdsk::ModelEqdsk(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData)
{
  // Step 1: Generate Model (with underlying simmetrix model)
  pGModel simModel = generateSimModel(planeMetaData, eqdskData, curvesMetaData);
  GM_write(simModel, "eqdsk.smd", 0, 0);  
  
  // Step 2: Save it as type Model.
  model.setSimModel(simModel);
}

const Model& ModelEqdsk::getModel() const
{
  return model;
}
