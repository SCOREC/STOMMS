#ifndef MESHMETADATA_H
#define MESHMETADATA_H

#include "modelData.h"

class PlaneMeshMetaData{
  public:
    void setModelPlane(const Plane& p);
  private:
    Plane modelPlane;
    std::vector <pGFace> modelFaces;  // vector of model faces on the poloidal plane.
    std::vector <int> faceAttributes;  // 0: No attribute,  1: oneElementDeepMesh, extend accordingly.
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    std::vector <std::vector<double>> meshVerticesLocation;
    pGVertex oPoint;  // model vertex on O-Point
    int planeNumber;  // plane number starting from 0 to numPlanes-1

    std::vector <double> setMeshVerticesOnFlux(const Flux& f);
    std::vector <int> setFaceAttributes(const std::vector <pGFace> geomFaces);
};

class MeshMetaData{
  public:
    MeshMetaData(const StommsModel& m);
  private:
    StommsModel stommsModel;
    std::vector <PlaneMeshMetaData> planeMeshData;
};

#endif
