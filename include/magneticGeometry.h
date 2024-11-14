#ifndef MAGNETICGEOMETRY_H
#define MAGNETICGEOMETRY_H

#include <vector>
#include "input.h"

class MagneticGeometry{
  public:
    MagneticGeometry(const args& a);
    VmecData getVmecData();
  private:
    VmecData vmec;  // Given VMEC data.
    BmwData bmw;  // Future
    EqdskData eqdsk;  // Future
}; 

#endif
