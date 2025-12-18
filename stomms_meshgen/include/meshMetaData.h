#ifndef MESHMETADATA_H
#define MESHMETADATA_H

#include "modelData.h"

// Class PlaneMeshMetaData holds the information required in meshing of individual poloidal planes.
class PlaneMeshMetaData{
  public:
    /*
     * Function to set the model plane in data.
     * const Plane& p (in): Input plane information from StommsModel. 
     */ 
    void setModelPlane(const Plane& p);

    /*
     * Function to return the vector of model faces on the particular poloidal plane.
     */ 
    const std::vector <Face>& getModelFacesOnPlane();

    /*
     * Function to get the vector holding the mesh type for each individual model face on the plane.
     */ 
    const std::vector <int>& getFaceMeshType();

    /*
     * Function to get a vector of flux curves on the poloidal plane.
     */ 
    const std::vector <Flux>& getFluxCurvesOnPlane();

    /*
     * Function to get a vector of set of points for the field following 
     * point placement on each flux curve of the plane.
     */ 
    const std::vector <std::vector<double>>& getMeshVerticesOnFlux();

    /*
     * Function to get model vertex on oPoint of  the poloidal plane.
     */ 
    const Vertex& getOPointOnPlane();

    /*
     * Function to get poloidal plane number.
     */ 
    const int& getPlaneNumber();
     
  private:
    Plane modelPlane;  // Plane information from StommsModel.
    std::vector <int> faceMeshType;  // 0: No mesh type specified,  1: oneElementDeepMesh, extend accordingly.
    std::vector <std::vector<double>> meshVerticesLocation;  // a vector to hold set of vertices on flux curves.

    /*
     * Function to set up the field following points on the flux curve.
     * const Flux& f (in): flux curve on which points are desired.
     * returns a vector of field following points on the flux curve.
     */ 
    std::vector <double> setMeshVerticesOnFlux(const Flux& f);   

    /*
     * Function to set up the mesh types for individual faces.
     * const std::vector <Face> geomFaces (in): input is vector of faces on which mesh type is required.
     * returns a vector of mesh type (int) for every model face in vector geomFaces.
     */  
    std::vector <int> setFaceMeshType(const std::vector <Face> geomFaces);
};

// class MeshMetaData holds all the planes containing their individual mesh meta data.
class MeshMetaData{
  public:
    /*
     * Contructor takes in StommdModel and set up planer mesh meta data one by one.
     */ 
    MeshMetaData(const StommsModel& m);

    /*
     * Function to return StommsModel.
     */ 
    const StommsModel& getStommsModel();

    /*
     * Function to get the vector of planes mesh meta data.
     */ 
    const std::vector <PlaneMeshMetaData>& getMeshMetaDataPlanes();
  private:
    StommsModel stommsModel;  // StommsModel that needed to be meshed.
    std::vector <PlaneMeshMetaData> planeMeshData;  // a vector of individual planes with their mesh meta data.
};

#endif
