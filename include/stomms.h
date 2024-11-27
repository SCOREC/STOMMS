#ifndef STOMMS_H
#define STOMMS_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelMetaData.h"
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
    void setPlanes(Model model, const args& a);    
    const std::vector <Plane>& getPlanes();

  private:
    MagneticGeometry mg;
    std::vector <PlaneMetaData> planesContainer;
    std::vector <Plane> planes;
    pProgress prog;  // Simmetrix handler to show execution details.
};
#endif

