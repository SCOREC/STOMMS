#ifndef STOMMS_H
#define STOMMS_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelMetaData.h"
#include "modeling.h"
#include "modelData.h"
#include "modelTopology.h"

class STOMMS{
  public:
    STOMMS(const MagneticGeometry& magGeom);
    ~STOMMS();
    void addPlane(PlaneMetaData pg);
    const std::vector <PlaneMetaData>& getPlanesContainer();
    /*
     * Setting model entities from simModel to respective planes.
     * pGModel model (in): the model created from modeling step.
     * std::vector <plane> planesContainer (out): the data for each plane  as object Plane is written to this container.
     * args* a (in): input parameters.
     */
  private:
    MagneticGeometry mg;
    std::vector <PlaneMetaData> planesContainer;
    pProgress prog;  // Simmetrix handler to show execution details.
};
#endif

