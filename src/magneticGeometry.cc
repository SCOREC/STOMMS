#include "magneticGeometry.h"

void MagneticGeometry::setMagneticGeometry(const args& a)
{
  if(a.vmecFileFound)
    vmec = a.in.vm; 
  else
    std::cout << "vmec data not found\n";

  psiAxis = a.psiAxis;
  psiLCFS = a.psiLCF;
}

const VmecData& MagneticGeometry::getVmecData()
{
  return vmec;
}

const double& MagneticGeometry::getPsiAxis()
{
  return psiAxis;
}
const double& MagneticGeometry::getPsiLCFS()
{
  return psiLCFS;
}
