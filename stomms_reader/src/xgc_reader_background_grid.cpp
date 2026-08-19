#include "xgc_reader_background_grid.hpp"

/****************************/
// Class EqdskGridData
/****************************/
EqdskGridData::EqdskGridData(const adios2::IO& io, const adios2::Engine& reader, std::string name)
              :ioGrid(io), readerGrid(reader), gridName(name)
{
  // Step 1: Validate the data 
  verifyGridData();

  // Step 2: Set psi field grid
  setPsiFieldOnGrid();

  // Step 3: Set 1D arrays (psi and poloidal current) of data
  setDataArrays();

  // Step 4: Set coordinates of different boundaries from the eqdsk file.
  setBoundaryData();
  
  // Step 5: Set splines coefficients.
  setSplineCoefficients();
}

// Function to verify that all required variables exist in the adios2 file.
void EqdskGridData::verifyGridData()
{
  // Step 1: Set the variables needed from the adios2 file.
  // Optional Variables: "boundaryPointsR", "boundaryPointsZ"
  std::vector <std::string> checkVariables = {"gridPointsR", "gridPointsZ",
                                              "limiterPointsR", "limiterPointsZ",
                                              "psi", "psiGrid", "domainBox", "psiSplineCoefficients",
                                              "poloidalCurrent", "poloidalCurrentSplineCoefficients",
                                              "psiAtChebyshevPoints", "bicubicSplineCoefficients"};

  // Step 2: Get the list of variables in group = gridName from adios2 file.
  std::vector <std::string> variables = findVarInGroup(ioGrid, gridName);

  // Step 3: Iterate over the required groups and if any of them can't be found, throw an
  // error message.
  for (const auto& var: checkVariables)
  {
    if (std::find(variables.begin(), variables.end(), gridName+ "/" + var) == variables.end())
      throw std::runtime_error("Error: Missing variable in background grid: " + var +".\n"
                          "Make sure all the variables are written in the adios file\n");
  }
}

// Function to set psi field on a grid from the adions2 file.
// Reads and sets the grid data (coordinates) and psi field on 
// those points.
void EqdskGridData::setPsiFieldOnGrid()
{
  // Step 1: Look for the grid points.
  std::string varName = gridName + "/gridPointsR";
  readAdios2Array(ioGrid, readerGrid, rGridPoints, varName);
  varName = gridName + "/gridPointsZ";
  readAdios2Array(ioGrid, readerGrid, zGridPoints, varName);

  // Step 2: Read the psi grid
  varName = gridName + "/psiGrid";
  readAdios2Array(ioGrid, readerGrid, psiGrid, varName);  

  assert (psiGrid.size() == rGridPoints.size()*zGridPoints.size());

  // Step 3: Set psi at Chebyshev points of each grid cell.
  setPsiAtChebyshevPoints();
}

// Function to set 1D arrays data to Eqdsk data.
// Current Support: Psi array and Poloidal Current array.
void EqdskGridData::setDataArrays()
{
  // Step 1: Read psi array.
  std::string varName = gridName + "/psi";
  readAdios2Array(ioGrid, readerGrid, psi, varName);

  // Step 2: Read poloidal current array.
  varName = gridName + "/poloidalCurrent";
  readAdios2Array(ioGrid, readerGrid, poloidalCurrent, varName);

  assert(psi.size() == poloidalCurrent.size());
}

// Function to set domain box limits. Its an array of size 4.
// Contains rMin, zMin, rMax, zMax.
void EqdskGridData::setDomainBox()
{
  // Step 1: Read the domain data from adios2 file.
  std::string varName = gridName + "/domainBox";
  readAdios2Array(ioGrid, readerGrid, domainBox, varName);
  assert (domainBox.size() == 4);
}

// Function to set coordinates of limiter (wall curve).
void EqdskGridData::setLimiter()
{
  // Step 1: Read the r coordinates of the wall curve.
  std::string varName = gridName + "/limiterPointsR";
  readAdios2Array(ioGrid, readerGrid, rLimiterPoints, varName);

  // Step 2: Read the z coordinates of the wall curve.
  varName = gridName + "/limiterPointsZ";
  readAdios2Array(ioGrid, readerGrid, zLimiterPoints, varName);

  assert(rLimiterPoints.size() == zLimiterPoints.size());
}

// Function to coordinates of plasma boundary from eqdsk file.
void EqdskGridData::setPlasmaBoundary()
{
  // Step 1: Read the r coordinates of the plasma boundary.
  std::string varName = gridName + "/boundaryPointsR";
  readAdios2Array(ioGrid, readerGrid, rBdryPoints, varName);

  // Step 2: Read the z coordinates of the plasma boundary.
  varName = gridName + "/boundaryPointsZ";
  readAdios2Array(ioGrid, readerGrid, zBdryPoints, varName);

  assert(rBdryPoints.size() == zBdryPoints.size());
}

// Function to set coordinates of different boundaries from the eqdsk file.
void EqdskGridData::setBoundaryData()
{
  // Step 1: Set the domain box.
  setDomainBox();

  // Step 2: Set the limiter coordinates.
  setLimiter();  

  // Step 3: Set the plasma boundary. 
  // Note: We might not need it since we already have separatrix curve information. 
  // Talk to XGC team about it.
  // setPlasmaBoundary();
}

// Function to set spline coefficients for psi grid.
// # of coefficients = 4*(# of r grid points)*(# of z grid points).
void EqdskGridData::setPsiSplineCoefficients()
{
  // Step 1: Read the spline array (3D) from adios2 file.
  // Save it as a vector.
  std::string varName = gridName + "/psiSplineCoefficients";
  readAdios2Array(ioGrid, readerGrid, psiSplineCoefficients, varName);

  assert(psiSplineCoefficients.size() == 4*rGridPoints.size()*zGridPoints.size());
}

// Function to set spline coefficients for poloidal current.
// # of coefficients = 2*(size of 1D poloidal current array).
void EqdskGridData::setPoloidalCurrentSplineCoefficients()
{
  // Step 1: Read the spline array (2D) from adios2 file.
  // Save it as a vector.
  std::string varName = gridName + "/poloidalCurrentSplineCoefficients";
  readAdios2Array(ioGrid, readerGrid, currentSplineCoefficients, varName);

  assert(currentSplineCoefficients.size() == 2*poloidalCurrent.size());
}

// Function to set bicubic spline coefficients for psi.
// # of coefficients = 16 * (# of r grid points - 1)*(# of z grid points - 1)
void EqdskGridData::setBicubicSplineCoefficients()
{
  // Step 1: Read the coefficients array (3D) from the adios2 file.
  std::string varName = gridName + "/bicubicSplineCoefficients";
  readAdios2Array(ioGrid, readerGrid, bicubicSplineCoefficients, varName);

  assert(bicubicSplineCoefficients.size() == 16*(rGridPoints.size() - 1)*(zGridPoints.size() - 1));
}

// Function to set psi values at Chebyshev points of each grid to evaluate bicubic splines.
void EqdskGridData::setPsiAtChebyshevPoints()
{
  // Step 1: Read the psi values at Chebyshev points on each gril cell.
  std::string varName = gridName + "/psiAtChebyshevPoints";
  readAdios2Array(ioGrid, readerGrid, psiAtChebyshevPoints, varName);

  assert(psiAtChebyshevPoints.size() == 16*(rGridPoints.size() - 1)*(zGridPoints.size() - 1));
}

// Function to set PSPLINE spline coefficients for eqdsk data.
// Sets psi spline and poloidal current spline coefficients.
void EqdskGridData::setSplineCoefficients()
{
  // Step 1: Set the psi spline coefficients.
  setPsiSplineCoefficients();

  // Step 2: Set the poloidal current spline coefficients.
  setPoloidalCurrentSplineCoefficients();

  // Step 3: Set the bicubic spline coefficients vector. 
  setBicubicSplineCoefficients();
}

// Function to return a vector of R coordinates of the grid. 
const std::vector <double>& EqdskGridData::getGridPointsR() const
{
  return rGridPoints;
}

// Function to return a vector of Z coordinates of the grid.
const std::vector <double>& EqdskGridData::getGridPointsZ() const
{
  return zGridPoints;
}

// Function to return a vector of psi field on grid points.
const std::vector <double>& EqdskGridData::getPsiFieldOnGrid() const
{
  return psiGrid;
}

// Function to return a vector of psi flux from Eqdsk.
const std::vector <double>& EqdskGridData::getPsiArray()const
{
  return psi;
}

// Function to return a vector of poloidal current from Eqdsk.
const std::vector <double>& EqdskGridData::getPoloidalCurrentArray() const
{
  return poloidalCurrent;
}

// Function to return a vector of R coordinates of the limiter.
const std::vector <double>& EqdskGridData::getLimiterR() const
{
  return rLimiterPoints;
}

// Function to return a vector of Z coordinates of the limiter.
const std::vector <double>& EqdskGridData::getLimiterZ() const
{
  return zLimiterPoints;
}

// Function to return eqdsk domain box.
std::array <double, 4> EqdskGridData::getDomainBox() const
{
  std::array <double, 4> box{domainBox[0], domainBox[1],
                             domainBox[2], domainBox[3]};
  return box;
}

// Function to return global PSPLINE psi spline coefficients.
const std::vector <double>& EqdskGridData::getPsplinePsiCoefficients() const
{
  return psiSplineCoefficients;
}

// Function to return PSPLINE poloidal current spline coefficients.
const std::vector <double>& EqdskGridData::getPsplineCurrentCoefficients() const
{
  return currentSplineCoefficients;
}

// Function to return a vector of all the bicubic spline coefficients 
// in each grid cell.
const std::vector <double>& EqdskGridData::getBicubicSplineCoefficients() const
{
  return bicubicSplineCoefficients;
}

// Function to return an array of bicubic spline coefficients for a grid cell.
// rIndex: index of grid cell along R. Ranges from 0 to rGridPoints.size()-1
// zIndex: index of grid cell along Z. Ranges from 0 to zGridPoints.size()-1
std::array <double, 16> EqdskGridData::getBicubicSplineCoefficientsInCell(int rIndex, int zIndex) const
{
  // Step 1: Get the number of cells in each direction (1 less than the total number in that direction)
  // Also, verify that the indices are valid.
  int numCellsAlongR = rGridPoints.size() - 1;
  int numCellsAlongZ = zGridPoints.size() - 1;

  if (rIndex >= numCellsAlongR || zIndex >= numCellsAlongZ)
  {
    throw std::runtime_error("Error in getBicubicSplineCoefficientsInCell: index out of range.\n"
                        "Range of rIndex should be: 0 - " + std::to_string(numCellsAlongR - 1) + ".\n" +
                        "Range of zIndex should be: 0 - " + std::to_string(numCellsAlongZ - 1) + ".\n" + 
                        "Given values: rIndex = " + std::to_string(rIndex) + " ,zIndex = " + std::to_string(zIndex));
  }

  // Step 2: Get the cell index and start point of the coefficients in bicubicSplineCoefficients.
  // Then copy it to the array.
  int cellIndex = (zIndex*numCellsAlongR) + rIndex;
  int start = 16*cellIndex;

  std::array <double, 16> coefficients;
  std::copy_n(bicubicSplineCoefficients.begin() + start, 16, coefficients.begin());
  return coefficients;
}

// Function to return an array of psi values at Chebyshev points for a grid cell.
std::array <double, 16> EqdskGridData::getPsiAtChebyshevPointsInCell(int rIndex, int zIndex) const
{
  // Step 1: Get the number of cells in each direction (1 less than the total number in that direction)
  // Also, verify that the indices are valid.
  int numCellsAlongR = rGridPoints.size() - 1;
  int numCellsAlongZ = zGridPoints.size() - 1;

  if (rIndex >= numCellsAlongR || zIndex >= numCellsAlongZ)
  {
    throw std::runtime_error("Error in getPsiAtChebyshevPointsInCell: index out of range.\n"
                        "Range of rIndex should be: 0 - " + std::to_string(numCellsAlongR - 1) + ".\n" +
                        "Range of zIndex should be: 0 - " + std::to_string(numCellsAlongZ - 1) + ".\n" + 
                        "Given values: rIndex = " + std::to_string(rIndex) + " ,zIndex = " + std::to_string(zIndex));
  }

  // Step 2: Get the cell index and start point in the vector psiAtChebyshevPoints.
  // Then copy it to the array.
  int cellIndex = (zIndex*numCellsAlongR) + rIndex;
  int start = 16*cellIndex;

  std::array <double, 16> psiValues;
  std::copy_n(psiAtChebyshevPoints.begin() + start, 16, psiValues.begin());
  return psiValues;
}
