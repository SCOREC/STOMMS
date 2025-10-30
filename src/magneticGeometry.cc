#include "magneticGeometry.h"

// A fucntion to set magnetic geometry information (any kind of physics information given)
// to class MagneticGeometry.
void MagneticGeometry::setMagneticGeometry(const args& a)
{
  // Step 1: Look for vmec file. If found read it, else return a warning.
  if(a.stellarator)
    vmec = a.in.vm; 
  else
    std::cout << "vmec data not found\n";

  psiAxis = a.psiAxis;
  psiLCFS = a.psiLCF;
}

// A function to return vmec data (struct VmecData)
const VmecData& MagneticGeometry::getVmecData()
{
  return vmec;
}

// A function to return psi value of the axis in the vmec domain.
const double& MagneticGeometry::getPsiAxis()
{
  return psiAxis;
}

// A function to return psi value of the last closed flux curve in the vmec domain.
const double& MagneticGeometry::getPsiLCFS()
{
  return psiLCFS;
}
