#ifndef MAGNETICGEOMETRY_H
#define MAGNETICGEOMETRY_H

#include <vector>
#include "input.h"

class MagneticGeometry{
  public:
    //MagneticGeometry(const args& a);
    void setMagneticGeometry(const args& a);
    const VmecData& getVmecData();
    const double& getPsiAxis();
    const double& getPsiLCFS();
  private:
    VmecData vmec;  // Given VMEC data.
    BmwData bmw;  // Future
    EqdskData eqdsk;  // Future
    double psiAxis;
    double psiLCFS;
}; 

#endif
