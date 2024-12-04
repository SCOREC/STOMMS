#ifndef MAGNETICGEOMETRY_H
#define MAGNETICGEOMETRY_H

#include <vector>
#include "input.h"

/*
 * A class to hold all the magnetic geometries. Only VMEC is supported 
 * at the moemnt. Reads the data from args class that handles all the 
 * input data.
 */
class MagneticGeometry{
  public:
    /*
     * A fucntion to set magnetic geometry information (any kind of physics information given)
     * to class MagneticGeometry.
     * const args& a (in): class holding all the input data (magnetic geometry, modeling and meshing parameters etc.)
     */ 
    void setMagneticGeometry(const args& a);

    /*
     * A function to return vmec data (struct VmecData).
     */ 
    const VmecData& getVmecData();

    /*
     * A function to return psi value of the axis in the vmec domain.
     */ 
    const double& getPsiAxis();

    /*
     * A function to return psi value of the last closed flux curve in the vmec domain.
     */ 
    const double& getPsiLCFS();
  private:
    VmecData vmec;  // Given VMEC data.
    BmwData bmw;  // Future task
    EqdskData eqdsk;  // Future task
    double psiAxis;  // psi value of the axis in the core region.
    double psiLCFS;  // psi value of the last closed flux curve in the core region.
}; 

#endif
