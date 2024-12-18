#ifndef MODELDATA_H
#define MODELDATA_H

#include "magneticGeometry.h"
#include "modelTopology.h"
#include "modelMetaData.h"
#include "modeling.h"
#include "input.h"
#include <util.h>
#include <map>

// A class to contain the information fo a flux curve. 
class Flux{
  public:
    int planeNumber;  // plane on which flux curve lies.
    double psiNormOnFlux;  // normalized psi value of flux curve
    std::vector <Edge> edgesOnFlux;
    int meshVerticesOnFlux;  // Number of vertices desired on flux curves
};

class Plane{
  public:
    std::vector <Face> modelFaces;
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    Vertex oPoint;
    int planeNumber;  // plane number starting from 0 to numPlanes-1
};

// The class StommsModel contains the model information after the creation of model from the meta data and
// also model information stored for each plane.
class StommsModel{
  public:
    /*
     * Constructor gets the model meta data and uses it to setup StommsModel.
     * const ModelMetaData& md (in): Takes the object of class ModelMetaData as input.
     */   
    StommsModel(const ModelMetaData& md);

    /*
     * Function to return the vector containing all the planes with their geometric data.
     */ 
    const std::vector <Plane>& getPlanes();

    /*
     * Function to return the underlying model in StommsModel.
     */ 
    const Model& getModel();

    /*
     * Function to return ModelMetaData stored in StommsModel.
     */ 
     const ModelMetaData& getModelMetaData();

  private:
    /*
     * Setting model entities from Simmetrix Model (pGModel) to respective planes.
     */
    void setPlanes();
    
    /*
     * Setting up attribues on model entities.
     */ 
    void setModelAttributes();
 
    /*
     * Setting up attributes on model faces.
     * Currently only pyhsics region type is set.
     * Extend it if needed in future for other attributes.
     */ 
    void setModelFaceAttributes();

    std::vector <int> faceAttributes; // a vector to hold face attributes (physics region type).
    ModelMetaData modelMetaData;  // Object of class ModelMetaData holding all the model meta data.
    std::vector <PlaneMetaData> planesContainer;  // a vector to hold all planes with their meta data.
    std::vector <Plane> planes;  // a vector to hold all the planes with their geometric data.
    Model model;  // underlying geometric model in StommsModel.
};

/*
 * From the Model, sort the O-point model vertices by planes.
 * Model model (in): the model created from modeling step.
 * std::vector <double> planeAngles (in): vector containing the toroidal angles of all the planes.
 * returns a map between plane number and corresponding O-point model vertex.
*/
std::map<int, Vertex> sortOPointsByPlanes(Model m, std::vector <double> planeAngles); 

/*
 * From the Model, sort the model faces by planes. 
 * Model model (in): the model created from modeling step.
 * std::vector <double> planeAngles (in): vector containing the toroidal angles of all the planes.
 * returns a map between plane number and a vector containing all the model faces on that plane.
*/
std::map<int,std::vector<Face>> sortFacesByPlanes(Model m, std::vector <double> planeAngles); 

/*
 * set all the flux curves on a plane.
 * Model model (in): the model created from modeling step.
 * int planeNum (in): the plane number on which flux curves will be set.
 * std::vector <PlaneMetaData> md (in): vector holding  all planes with their meta data.
 * returns a vector of flux curves (type Flux).
*/
std::vector<Flux> setFluxCurvesOnPlanes(Model m, int planeNum, std::vector <PlaneMetaData> md);

/*
 * Function to check if a model face in on core region or not.
 * const Face &face (in): face to be checked.
 * returns true if face is on core region, else false.
 */
bool isFaceOnCore(const Face& f);

/*
 * Future Task**.
 * Function to check if a model face in on SOL region or not.
 * const Face &face (in): face to be checked.
 * returns true if face is on SOL region, else false.
 */
bool isFaceOnSOL(const Face& f);

/*
 * Future Task**.
 * Function to check if a model face in on private region or not.
 * const Face &face (in): face to be checked.
 * returns true if face is on private region, else false.
 */
bool isFaceOnPVT(const Face& f);

/*
 * Function to get model face physics type.
 * const Face &face (in): face on physics type is required.
 * retunrs the physics type as int (detail below).
 * 0: Not classified.
 * 1: core region.
 * 2: SOL.
 * 3: Private region. Extend accordingly.
 */ 
int getModelFacePhysicsType(const Face &face);

#endif
