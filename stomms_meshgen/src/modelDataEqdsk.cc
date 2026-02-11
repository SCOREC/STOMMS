#include "modelDataEqdsk.h"

ModelEqdsk::ModelEqdsk(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData)
{
  // Step 1: Generate Model (with underlying simmetrix model)
  model = generateModelEqdsk(planeMetaData, curvesMetaData);
  
  // Step 2: Any postprocessing if needed

}
