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
  double rStepSize = (rMax - rMin)/(numR);
  for (int i = 0; i <= numR; i++)
    rPoints.push_back(rMin + i*rStepSize);

  // Step 2: Set z points.
  double zStepSize = (zMax - zMin)/(numZ);
  for (int i = 0; i <= numZ; i++)
    rPoints.push_back(zMin + i*zStepSize);

  // Step 3: Set z points.
  double phiMax = 360.00/nFieldPeriods;
  double phiStepSize = ( phiMax - 0.0)/(numPhi);
  for (int i = 0; i <= numPhi; i++)
    phiPoints.push_back(0.0 + i*phiStepSize);
}
