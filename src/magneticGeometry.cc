#include "magneticGeometry.h"

/***********************************************/
// Class: MagneticGeometry
// Base class for magnetic geometry
/***********************************************/
const std::map<int,std::vector<PhysicsPoint>>& MagneticGeometry::getOPoints() const
{
  return oPoints;
}

const std::map<int,std::vector<PhysicsPoint>>& MagneticGeometry::getXPoints() const
{
  return xPoints;
}

const VmecData& MagneticGeometry::getVmecData() const
{
  return vmec;
}

const EqdskData& MagneticGeometry::getEqdskData() const
{
  return eqdsk;
}

/***********************************************/
// Class: MagneticGeometryForStellarator
// Derived class for MagneticGeometry
/***********************************************/

/*
 * A fucntion to set magnetic geometry information (any kind of physics information given)
 * to class MagneticGeometry.
 */
MagneticGeometryForStellarator::MagneticGeometryForStellarator(const std::string& vmecFileName):vmecFile(vmecFileName)
{
  // Step 1: Read VMEC file data
  vmec = readVmecData();
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
double MagneticGeometryForStellarator::getPsiLCFS() const
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

/***********************************************/
// Class: MagneticGeometryForTokamak
// Derived class for MagneticGeometry
/***********************************************/
MagneticGeometryForTokamak::MagneticGeometryForTokamak(const WallCurve& wall)
{
  // Step 1: Find critical points from EQDSK file and print them.
  wallCurve = wall;
  CriticalPointsEqdsk criticalPoints(wall);

  printCriticalPoints(criticalPoints.getOPoints());
  printCriticalPoints(criticalPoints.getXPoints());

  // Step 2: Setup critical points on each plane.
  // Just one plane for tokamak
  oPoints[0] = criticalPoints.getOPoints();
  xPoints[0] = criticalPoints.getXPoints();  
}

/* 
 * Function to set magnetic geometry.
 */
std::unique_ptr <MagneticGeometry> setMagneticGeometry(const args& input, const PhysicalGeometry& physicalGeometry)
{
  std::unique_ptr <MagneticGeometry> mg;

  // Step 1: If reactor type is stellarator, look for VMEC file and set
  // magnetic geometry using MagneticGeometryForStellarator.
  if(input.getReactorType() == ReactorType::Stellarator)
  {
    std::cout << "Geometry (Reactor) Type: Stellarator\n"; 
    std::string vmecFileName;

    // Step 1.1: If can't find VMEC file, throw an error.
    if (!input.getVmecFile().empty())
    {
      vmecFileName = input.getVmecFile();
      std::cout << "Input VMEC file: " << vmecFileName << "\n";
    }
    else
    {
      std::cerr << "ERROR: VMEC file not found. Make sure the name or path to file is correct\n";
      exit(1);
    }
    
    // Step 1.2: Set up the magnetic geometry
    mg =  std::make_unique<MagneticGeometryForStellarator>(vmecFileName);
  }

  // Step 2: If reactor type is tokamak, look for EQDSK file and set magnetic 
  // geometry using MagneticGeometryForTokamak.
  if(input.getReactorType() == ReactorType::Tokamak)
  {
    std::cout << "Geometry (Reactor) Type: Tokamak \n";
    int planeNum = 0; // for tokamaks
    WallCurve wall = physicalGeometry.getWallCurveAtPlane(planeNum);
    mg =  std::make_unique<MagneticGeometryForTokamak>(wall);
    
  }

  return mg;
}
