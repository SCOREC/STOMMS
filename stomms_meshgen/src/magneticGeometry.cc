#include "magneticGeometry.h"
#include "magneticGeometryStellarator.h"
#include "magneticGeometryTokamak.h"

/* 
 * Function to set magnetic geometry.
 */
std::unique_ptr <MagneticGeometry> setMagneticGeometry(const Inputs& input, const PhysicalGeometry& physicalGeometry, const ModelMetaData& modelMetaData)
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
    mg =  std::make_unique<MagneticGeometryForStellarator>(modelMetaData, vmecFileName);
  }

  // Step 2: If reactor type is tokamak, look for EQDSK file and set magnetic 
  // geometry using MagneticGeometryForTokamak.
  if(input.getReactorType() == ReactorType::Tokamak)
  {
    std::cout << "Geometry (Reactor) Type: Tokamak \n";
    int planeNum = 0; // for tokamaks
    WallCurve wall = physicalGeometry.getWallCurveAtPlane(planeNum);
    mg =  std::make_unique<MagneticGeometryForTokamak>(modelMetaData, wall, input);
  }

  return mg;
}
