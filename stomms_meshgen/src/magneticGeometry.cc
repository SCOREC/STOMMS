#include "magneticGeometry.h"
#include "magneticGeometryVmec.h"
#include "magneticGeometryEqdsk.h"
#include "magneticGeometryBmw.h"

/** 
 * Function to set magnetic geometry.
 */
std::unique_ptr <MagneticGeometry> setMagneticGeometry(const Inputs& input, const PhysicalGeometry& physicalGeometry, const ModelMetaData& modelMetaData)
{
  std::unique_ptr <MagneticGeometry> mg;

  // Step 1: If magnetic field source is VMEC, look for VMEC file and set
  // magnetic geometry using MagneticGeometryVmec.
  if(input.getMagneticFieldInputSource() == MagneticInputSource::Vmec)
  {
    std::cout << "Geometry (Reactor) Type: Stellarator\n";
    std::cout << "Magnetic Field Input Source: VMEC\n"; 
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
    mg =  std::make_unique<MagneticGeometryVmec>(modelMetaData, vmecFileName);
  }

  // Step 2: If magnetic field source is BMW, look for BMW file and set magnetic 
  // geometry using MagneticGeometryBmw.
  if(input.getMagneticFieldInputSource() == MagneticInputSource::Bmw)
  {
    std::cout << "Geometry (Reactor) Type: Stellarator\n";
    std::cout << "Magnetic Field Input Source: BMW\n";
    std::string bmwFileName;

    // Step 1.1: If can't find Bmw file, throw an error.
    if (!input.getBmwFile().empty())
    {
      bmwFileName = input.getBmwFile();
      std::cout << "Input BMW file: " << bmwFileName << "\n";
    }
    else
    {
      std::cerr << "ERROR: BMW file not found. Make sure the name or path to file is correct\n";
      exit(1);
    }
    
    // Step 1.2: Set up the magnetic geometry
    mg =  std::make_unique<MagneticGeometryBmw>(modelMetaData, bmwFileName);
  }

  // Step 3: If magnetic field source in EQDSK, look for EQDSK file and set magnetic 
  // geometry using MagneticGeometryEqdsk.
  if(input.getMagneticFieldInputSource() == MagneticInputSource::Eqdsk)
  {
    std::cout << "Geometry (Reactor) Type: Tokamak \n";
    std::cout << "Magnetic Field Input Source: EQDSK\n";
    int planeNum = 0; // for tokamaks
    WallCurve wall = physicalGeometry.getWallCurveAtPlane(planeNum);
    mg =  std::make_unique<MagneticGeometryEqdsk>(modelMetaData, wall, input);
  }

  return mg;
}
