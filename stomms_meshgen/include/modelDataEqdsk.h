#ifndef MODELDATAEQDSK_H
#define MODELDATAEQDSK_H

#include "modelingEqdsk.h"
#include <unordered_map>

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

    /**
     * Function to set a map between model tags (int) and mesh size on that model face.
     * @param modelFaces: a vector of all the model faces.
     * @param eqdsk: EqdskData object to have access to eqdsk and pspline function calls.
     * @param meshSize: General unstrucutred mesh size for the plane.
     * @return a map between model tags (int) and mesh size on that model face
     */ 
    std::unordered_map <int, double>setMeshSizesOnModelFaces(const std::vector <Face>& modelFaces,
    							     EqdskData& eqdsk, double meshSize);
};

#endif
