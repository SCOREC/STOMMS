#ifndef MESHMETADATA_H
#define MESHMETADATA_H

#include "stommsModel.h"

// Class PlaneMeshMetaData holds the information required in meshing of individual poloidal planes.
class PlaneMeshMetaData{
  public:
    /**
     * Function to set the model plane in data.
     * @param p: Input plane information from StommsModel. 
     */ 
    void setModelPlane(const Plane& p);

    /**
     * Function to return the vector of model faces on the particular poloidal plane.
     */ 
    const std::vector <Face>& getModelFacesOnPlane();

    /**
     * Function to get the vector holding the mesh type for each individual model face on the plane.
     */ 
    const std::vector <int>& getFaceMeshType();

    /**
     * Function to get a vector of flux curves on the poloidal plane.
     */ 
    const std::vector <Flux>& getFluxCurvesOnPlane();

    /**
     * Function to get a vector of set of the class FluxParametricPoints. This vector is
     * equal to the number of the flux curves. Each member contains the details of the 
     * field following points parametric values.
     */ 
    const std::vector <FluxParametricPoints>& getMeshVerticesOnFlux() const;
 
    /**
     * Function to get model vertex on oPoint of  the poloidal plane.
     */ 
    const Vertex& getOPointOnPlane();

    /**
     * Function to get poloidal plane number.
     */ 
    const int& getPlaneNumber();
    
    /**
     * Function to get unstructured mesh size on the plane.
     */
    const double& getUnstructuredMeshSizeOnPlane() const;

    /**
     * Function to get mesh size on the model face of the plane.
     * @param f: model face on which mesh size is desired.
     * @return mesh size desired on the model face.
     */  
     double getMeshSizeOnModelFace(const Face& f);
  private:
    Plane modelPlane;  // Plane information from StommsModel.
    std::vector <int> faceMeshType;  // 0: No mesh type specified,  1: oneElementDeepMesh, extend accordingly.
    std::vector <FluxParametricPoints> meshVerticesParametricLocation;  // parametric values of vertices      
    double meshSizeUnstructured;    
    std::unordered_map <int , double> meshSizesOnModelFace;  // map between model face tag (int) and mesh size   

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
