#include "magneticGeometryStellarator.h"

/*
 * A fucntion to set magnetic geometry information (any kind of physics information given)
 * to class MagneticGeometry.
 */
MagneticGeometryForStellarator::MagneticGeometryForStellarator(const ModelMetaData& md, const std::string& vmecFileName):vmecFile(vmecFileName)
{
  // Step 1: Read VMEC file data
  vmec = readVmecData();

  // Step 2: Get the vector of planes with their meta data from ModelMetadata.
  modelVmec = ModelVmec(md, vmec);
}

/*
 * Reads input VMEC file and store relevant data in struct vmecData.
 */
VmecData MagneticGeometryForStellarator::readVmecData()
{
  VmecData v;

  // Step 1: Read all the variables in vmec file
  NcFile vFile(vmecFile, NcFile::read);

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

/* 
 * A function to return psi value of the axis in the vmec domain.
 */
double MagneticGeometryForStellarator::getPsiAxis() const
{
  return vmec.psi[0];
}

/* 
 * A function to return psi value of the last closed flux curve in the vmec domain.
 */
double MagneticGeometryForStellarator::getPsiCoreBoundary() const
{
  return vmec.psi[vmec.nSurf - 1]; 
}

/*
 * Function to return the reactor type(Stellarator for this class).
 */
ReactorType MagneticGeometryForStellarator::getReactorType() const
{
  return ReactorType::Stellarator;
}

// Function to get a map between plane number and vector of OPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryForStellarator::getOPoints() const
{
  return oPoints;
}

// Function to get a map between plane number and vector of XPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryForStellarator::getXPoints() const
{
  return xPoints;
}

const Model& MagneticGeometryForStellarator::getModel() const
{
  return modelVmec.getModel();
}

const std::vector <Plane>& MagneticGeometryForStellarator::getPlanes() const
{
  return modelVmec.getPlanes();
}
