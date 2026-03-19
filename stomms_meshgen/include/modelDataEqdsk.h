#ifndef MODELDATAEQDSK_H
#define MODELDATAEQDSK_H

#include "modelingEqdsk.h"

class ModelEqdsk{
  public:
    ModelEqdsk(){};
    ModelEqdsk(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData);
    const Model& getModel() const;
    const std::vector <Plane>& getPlanes() const;
  private:
    Model model;  // Model class. It has underlying Simmetrix model too.
    EqdskData eqdsk;
    std::vector <Plane> planes;  // a vector to hold all the planes. For tokamak, = 1
    CurveContainer curvesContainer;

    std::vector <Flux> setFluxCurvesOnPlane();
    std::vector <Face> setModelFacesOnPlane();
    void setPlane();
    void setMeshVerticesOnPlanes();
    void finalModelFaceClassification();
    void setMeshVerticesOnPlane(int planeIndex);
};

#endif
