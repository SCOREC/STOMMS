#ifndef STOMMS_H
#define STOMMS_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelMetaData.h"
#include "modelTopology.h"

class STOMMS{
  public:
    STOMMS(const MagneticGeometry& magGeom);
    ~STOMMS();
    void addPlane(PlaneMetaData pg);
    const std::vector <PlaneMetaData>& getPlanesContainer();

  private:
    MagneticGeometry mg;
    std::vector <PlaneMetaData> planes;
    pProgress prog;  // Simmetrix handler to show execution details.
};
#endif

