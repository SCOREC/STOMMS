#include "magneticGeometry.h"

// A fucntion to set magnetic geometry information (any kind of physics information given)
// to class MagneticGeometry.
void MagneticGeometry::setMagneticGeometry(const args& a)
{
  // Step 1: Look for vmec file. If found read it, else return a warning.
  if(a.getReactorType() == ReactorType::Stellarator)
  {
    vmecFileName = a.getVmecFile();
    vmec = readVmecData();
  }
  else
    std::cout << "vmec data not found\n";

  psiAxis = vmec.psi[0];
  psiLCFS = vmec.psi[vmec.nSurf - 1];
}

// Read input VMEC file and store relevant data in struct vmecData.
VmecData MagneticGeometry::readVmecData()
{
  VmecData v;

  // Step 1: Read all the variables in vmec file
  NcFile vFile(vmecFileName, NcFile::read);

  // netCDF Variables. First four hold single values and the rest are arrays of data.
  NcVar vmecMinor, vmecMajor, vmecSurf, vmecMode, vmecR, vmecZ, vmecL, vmecXm, vmecXn, vmecPsi, vmecIota;

  // Step 2: Read required variables from vmec file
  vmecMinor = vFile.getVar("Aminor_p");
  vmecMajor = vFile.getVar("Rmajor_p");
  vmecSurf = vFile.getVar("ns");
  vmecMode = vFile.getVar("mnmax");
  vmecR = vFile.getVar("rmnc");
  vmecZ = vFile.getVar("zmns");
  vmecL = vFile.getVar("lmns");
  vmecXm = vFile.getVar("xm");
  vmecXn = vFile.getVar("xn");
  vmecPsi = vFile.getVar("phi");
  vmecIota = vFile.getVar("iotaf");

  // Step 3: Assign netCDF variables to local variables and arrays
  vmecMinor.getVar(&v.minorR);
  vmecMajor.getVar(&v.majorR);
  vmecSurf.getVar(&v.nSurf);
  vmecMode.getVar(&v.nMode);

  v.R.resize(v.nSurf*v.nMode);
  v.Z.resize(v.nSurf*v.nMode);
  v.L.resize(v.nSurf*v.nMode);
  v.xm.resize(v.nMode);
  v.xn.resize(v.nMode);
  v.psi.resize(v.nSurf);
  v.iota.resize(v.nSurf);

  vmecR.getVar(v.R.data());
  vmecZ.getVar(v.Z.data());
  vmecL.getVar(v.L.data());
  vmecXm.getVar(v.xm.data());
  vmecXn.getVar(v.xn.data());
  vmecPsi.getVar(v.psi.data());
  vmecIota.getVar(v.iota.data());

  // Step 4: Return vmec data.
  return v;
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
