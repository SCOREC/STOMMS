#include "bmwData.h"

/***********************************************/
// Class BmwData
// Takes in a bmw file, and populate the class
// with magnetic field and background grid data
/***********************************************/

BmwData::BmwData(const std::string& bmwFileName)
{
  // Step 1: Set bmw file name in the class
  NcFile bmwFile(bmwFileName, NcFile::read);

  // Step 2: Set physical grid data
  setGridData(bmwFile);

  // Step 3: Set Magnetic Field Data
  setMagneticFieldData(bmwFile);

  // Step 4: Interpolate the magnetic field
  int numPhiLocal = numPhi + 1;  // extra point for periodic BC
  init_b_spline_(rPoints.data(), zPoints.data(), phiPointsRadian.data(), &numR, 
                 &numZ, &numPhiLocal, bR.data(), bZ.data(), bPhi.data());
}

void BmwData::setGridData(const NcFile& bmwFile)
{
  // Step 1: Declare netcdf variables to read data from netcdf bmw file.
  NcDim numRPoints, numZPoints, numPhiPoints;  // saved as dimensions in bmw file
  NcVar minR, minZ, maxR, maxZ, nfp;  // saved as variables

  // Step 2: Read the variables from bmw file.
  numRPoints = bmwFile.getDim("r");
  numZPoints = bmwFile.getDim("z");
  numPhiPoints = bmwFile.getDim("phi");
  minR = bmwFile.getVar("rmin");
  minZ = bmwFile.getVar("zmin");
  maxR = bmwFile.getVar("rmax");
  maxZ = bmwFile.getVar("zmax");
  nfp = bmwFile.getVar("nfp");

  // Step 3: Assign the data to local class variables.
  numR = numRPoints.getSize();
  numZ = numZPoints.getSize();
  numPhi = numPhiPoints.getSize();
  minR.getVar(&rMin);
  minZ.getVar(&zMin);
  maxR.getVar(&rMax);
  maxZ.getVar(&zMax);
  nfp.getVar(&nFieldPeriods);

  // Step 4: Set Grid point vectors.
  setGridPoints(); 
}

void BmwData::setMagneticFieldData(const NcFile& bmwFile)
{
  // Step 1: Declare netcdf variables to read data from netcdf bmw file.
  NcVar bRGrid, bZGrid, bPhiGrid;

  // Step 2: Read the variables from bmw file.
  bRGrid = bmwFile.getVar("br_grid");
  bZGrid = bmwFile.getVar("bz_grid");
  bPhiGrid = bmwFile.getVar("bp_grid");

  // Step 3: Assign the data to local class variables.
  // Set size of vectors for grid data (numR*numZ*numPhi).
  // Then assign the values.
  bR.resize(numR*numZ*numPhi);
  bZ.resize(numR*numZ*numPhi);
  bPhi.resize(numR*numZ*numPhi);

  bRGrid.getVar(bR.data());
  bZGrid.getVar(bZ.data());
  bPhiGrid.getVar(bPhi.data());
}

void BmwData::setGridPoints()
{
  // Step 1: Set r points.
  double rStepSize = (rMax - rMin)/(numR - 1);
  for (int i = 0; i < numR; i++)
    rPoints.push_back(rMin + i*rStepSize);

  // Step 2: Set z points.
  double zStepSize = (zMax - zMin)/(numZ - 1);
  for (int i = 0; i < numZ; i++)
    zPoints.push_back(zMin + i*zStepSize);

  // Step 3: Set phi points and also save them in radians.
  double phiMax = 360.00/nFieldPeriods;
  double phiStepSize = (phiMax - 0.0)/(numPhi);
  for (int i = 0; i < numPhi; i++)
  {
    phiPoints.push_back(i*phiStepSize);
    phiPointsRadian.push_back(i*phiStepSize*M_PI/180.0);
  }

  // Step 4: Add last phi point to the vector for spline interpolation requirement.
  phiPointsRadian.push_back(phiMax*M_PI/180.0);
}

const int& BmwData::getNumPointsR() const
{
  return numR;
}

const int& BmwData::getNumPointsZ() const
{
  return numZ;
}

const int& BmwData::getNumPointsPhi() const
{
  return numPhi;
}

std::array <double, 4> BmwData::getDomainBox() const
{
  std::array <double, 4> domainBox = {rMin, zMin, rMax, zMax};
  return domainBox;
}

const std::vector <double>& BmwData::getPointsR() const
{
  return rPoints;
}

const std::vector <double>& BmwData::getPointsZ() const
{
  return zPoints;
}

const std::vector <double>& BmwData::getPointsPhi() const
{
  return phiPoints;
}

const std::vector <double>& BmwData::getFieldBr() const
{
  return bR;
}

const std::vector <double>& BmwData::getFieldBz() const
{
  return bZ;
}

const std::vector <double>& BmwData::getFieldBphi() const
{
  return bPhi;
}

std::array <double, 3> BmwData::getMagneticFieldAtPoint(double r, double z, double phi) const
{
  // Step 1: Check R-Z bounds
  if (r < rMin || r > rMax || z < zMin || z > zMax)
  {
    std::string errorMessage = "The requested point ( " + std::to_string(r) + ", " + std::to_string(z) + 
                               "is outside the bounds. The bounding box is:\n" +
                               "rMin = " + std::to_string(rMin) + ", rMax = " + std::to_string(rMax) +
                               "zMin = " + std::to_string(zMin) + ", zMax = " + std::to_string(zMax) + "\n"; 
    throw std::out_of_range(errorMessage);
  }

  // Step 2: Check the phi bounds and map the phi value (due to symmetry) to first field period.
  // Once mapped convert it to radians.
  // Step 2.1: Check bounds.
  if (phi < 0.0 || phi > 360.0)
  {
    std::string errorMessage = "The requested toroidal angle phi = " + std::to_string(phi) + 
                               "is outside the bounds. The toroidal bounds are:\n" +
                               "phiMin = 0.0, phiMax = 360.00\n";
    throw std::out_of_range(errorMessage);
    
  }

  // Step 2.2: Map the phi to 0-phiFieldPeriod and convert phi to radians.
  const double phiFieldPeriod = 360.0/nFieldPeriods;
  phi = std::fmod(phi, phiFieldPeriod);
  const double phiRadians = phi*M_PI/180.0;

  // Step 3: Get the magnetic field array.
  std::array <double, 3> bVector{};
  int ier = 0;

  eval_b_field_(&r, &z, &phiRadians, bVector.data(), &ier);
  
  if (ier)
  {
    std::string errorMessage = "Magnetic field interpolation failed for BMW data\n";
    throw std::runtime_error(errorMessage);
  }

  return bVector;
}
