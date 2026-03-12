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
    std::vector <Plane> planes;  // a vector to hold all the planes with their geometric data.
    Model model;  // underlying geometric model in StommsModel.
    std::shared_ptr<MagneticGeometry> magneticGeometry;  // Magnetic geometry information.
};

/**
 * Function to get model face physics type.
 * @param face: face on physics type is required.
 * @return the physics type.
 */ 
FaceType getModelFacePhysicsType(const Face &face);

#endif
