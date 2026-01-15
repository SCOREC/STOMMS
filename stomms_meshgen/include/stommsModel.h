#ifndef STOMMSMODEL_H
#define STOMMSMODEL_H

#include "magneticGeometry.h"
#include <util.h>
#include <map>

// The class StommsModel contains the model information after the creation of model and
// also model information stored for each plane.
class StommsModel{
  public:
    /**
     * Constructor gets the model meta data and uses it to setup StommsModel.
     * @param mg: magnetic geometry (vmec, eqdsk, bmw etc.) with underlying model information.
     */   
    StommsModel(std::shared_ptr<MagneticGeometry> mg);

    /**
     * Function to return the vector containing all the planes with their geometric data.
     */ 
    const std::vector <Plane>& getPlanes();

    /**
     * Function to return the underlying model in StommsModel.
     */ 
    const Model& getModel();

  private:
    /**
     * Setting up attribues on model entities.
     */ 
    void setModelAttributes();
 
    /**
     * Setting up attributes on model faces.
     * Currently only pyhsics region type is set.
     * Extend it if needed in future for other attributes.
     */ 
    void setModelFaceAttributes();

    std::vector <int> faceAttributes; // a vector to hold face attributes (physics region type).
    std::vector <Plane> planes;  // a vector to hold all the planes with their geometric data.
    Model model;  // underlying geometric model in StommsModel.
    std::shared_ptr<MagneticGeometry> magneticGeometry;  // Magnetic geometry information.
};

/**
 * Function to check if a model face in on core region or not.
 * @param face: face to be checked.
 * @return true if face is on core region, else false.
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

/**
 * Function to get model face physics type.
 * @param face: face on physics type is required.
 * @retunr the physics type as int (detail below). <br>
 * 0: Not classified. <br>
 * 1: core region. <br>
 * 2: SOL. <br>
 * 3: Private region. Extend accordingly. 
 */ 
int getModelFacePhysicsType(const Face &face);

#endif
