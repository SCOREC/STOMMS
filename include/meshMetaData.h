#ifndef MESHMETADATA_H
#define MESHMETADATA_H

#include "modelData.h"

class PlaneMeshMetaData{
  public:
    void setModelPlane(const Plane& p);
    const std::vector <Face>& getModelFacesOnPlane();
    const std::vector <int>& getFaceAttributesOnPlane();
    const std::vector <Flux>& getFluxCurvesOnPlane();
    const std::vector <std::vector<double>>& getMeshVerticesOnFlux();
    const Vertex& getOPointOnPlane();
    const int& getPlaneNumber();
     
  private:
    Plane modelPlane;
    std::vector <int> faceAttributes;  // 0: No attribute,  1: oneElementDeepMesh, extend accordingly.
    std::vector <std::vector<double>> meshVerticesLocation;
    std::vector <double> setMeshVerticesOnFlux(const Flux& f);
    std::vector <int> setFaceAttributes(const std::vector <Face> geomFaces);
};

class MeshMetaData{
  public:
    MeshMetaData(const StommsModel& m);
    const StommsModel& getStommsModel();
    const std::vector <PlaneMeshMetaData>& getMeshMetaDataPlanes();
  private:
    StommsModel stommsModel;
    std::vector <PlaneMeshMetaData> planeMeshData;
};

#endif
