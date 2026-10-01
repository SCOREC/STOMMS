#include "magneticGeometryBmw.h"

/***********************************************/
// class MagneticGeometryBmw
// Geometry construction along with the physics
// properties of stellarators are done here.
/***********************************************/
MagneticGeometryBmw::MagneticGeometryBmw(const ModelMetaData& md, const std::string& bmwFileName)
{
  // Step 1: Read the BMW data
  bmwFile = bmwFileName;
  bmw = BmwData(bmwFile);
}

// A function to return psi value of the axis in the bmw domain.
double MagneticGeometryBmw::getPsiAxis() const
{
  return 0.0;
}

// A function to return psi value of the last closed flux curve in the bmw domain.
double MagneticGeometryBmw::getPsiCoreBoundary() const
{
  return 0.0; 
}

// Function to return the reactor type(Stellarator for this class).
ReactorType MagneticGeometryBmw::getReactorType() const
{
  return ReactorType::Stellarator;
}

// Function to get a map between plane number and vector of OPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryBmw::getOPoints() const
{
  return oPoints;
}

// Function to get a map between plane number and vector of XPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryBmw::getXPoints() const
{
  return xPoints;
}

// Function to get model associated with stellarator geometry.
const Model& MagneticGeometryBmw::getModel() const
{
  return dummyModel;
}

// Function to get all the geometric information on individual planes.
const std::vector <Plane>& MagneticGeometryBmw::getPlanes() const
{
  return dummyPlanes;
}

// Function to return magnetic field data on the background grid.
const GridFieldData& MagneticGeometryBmw::getGridFieldData() const
{
  return gridData;
}
