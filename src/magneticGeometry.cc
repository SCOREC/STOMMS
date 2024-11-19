#include "magneticGeometry.h"

MagneticGeometry::MagneticGeometry(const args& a)
{
  if(a.vmecFileFound)
    vmec = a.in.vm; 
  else
    std::cout << "vmec data not found\n";
}

const VmecData& MagneticGeometry::getVmecData()
{
  return vmec;
}
