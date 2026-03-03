#ifndef MODELDATAEQDSK_H
#define MODELDATAEQDSK_H

#include "modelingEqdsk.h"

class ModelEqdsk{
  public:
    ModelEqdsk(){};
    ModelEqdsk(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData);
    const Model& getModel() const;
  private:
    ModelMetaData modelMetaData;  // Object of class ModelMetaData holding all the model meta data.
    Model model;  // Model class. It has underlying Simmetrix model too.
    EqdskData eqdsk;
};

#endif
