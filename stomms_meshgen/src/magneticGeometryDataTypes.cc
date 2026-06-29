#include "magneticGeometryDataTypes.h"

/***********************************************/
// Class: Flux
/***********************************************/

// To set a model edge to the flux curve.
void Flux::setSimEdgeToFlux(pGEdge ge)
{
  Edge modelEdge;
  modelEdge.setSimEdge(ge);
  edgesOnFlux.push_back(modelEdge);
}

/***********************************************/
// Class: CurveContainer
/***********************************************/

// Constructor to set flux curves already been generated. Takes in set of closed flux curves, 
// separatrices and wall curve.
CurveContainer::CurveContainer(std::vector <Flux>& closedCurves, std::vector <Flux>& separatrices, WallCurve& wall)
{
  // Step 1: Set curves
  curvesClosed = closedCurves;
  curvesSeparatrix = separatrices;
  wallCurve = wall;
}

// Function to set critical points in the container.
void CurveContainer::setCriticalPoints(const std::vector <PhysicsPoint>& oPts, const std::vector <PhysicsPoint>& xPts)
{
  oPoints = oPts;
  xPoints = xPts;
}

// Function to set open flux curves in the container.
void CurveContainer::setOpenCurves(const std::vector <Flux>& openFluxCurves)
{
  openCurves = openFluxCurves;
}

// Function to set wall curve edges in the container.
void CurveContainer::setWallEdges(const std::vector <Edge>& edgesOnWall)
{
  wallEdges = edgesOnWall;
}

// Function to get a vector of closed flux curves in the container. 
std::vector <Flux>& CurveContainer::getCurvesClosed()
{
  return curvesClosed;
}

// Function to get a vector of separatrix curves in the container.
std::vector <Flux>& CurveContainer::getCurvesSeparatrix() 
{
  return curvesSeparatrix;
}

// Function to get a vector of open curves in the container.
std::vector <Flux>& CurveContainer::getCurvesOpen()
{
  return openCurves;
}

// Function to get wall curve.
WallCurve& CurveContainer::getWallCurve()
{
  return wallCurve;
}

// Function to return vector of model edges on the wall.
const std::vector <Edge>& CurveContainer::getWallEdges()
{
  return wallEdges;
}

// Function to get a vector of O-points.
const std::vector <PhysicsPoint>& CurveContainer::getOPoints() const
{
  return oPoints;
}

// Function to get a vector of X-points.
const std::vector <PhysicsPoint>& CurveContainer::getXPoints() const
{
  return xPoints;
}

/***********************************************/
// Class GridData
/***********************************************/

// Constructor to create a physical grid. It adds no field data to grid.
GridFieldData::GridFieldData(const std::vector <double>& pointsR, const std::vector <double>& pointsZ)
{
  rPoints = pointsR;
  zPoints = pointsZ; 
}

// Function to set a field of type double on the grid.
void GridFieldData::setDoubleFieldOnGrid(const std::vector <double>& field, const FieldType& fieldType)
{
  int numGridPoints = rPoints.size()*zPoints.size();
  if (field.size() != numGridPoints)
  {
    std::cout << "ERROR: The field data doesn't match the grid\n";
    std::cout << "The number of grid points = " << rPoints.size() << "*" << zPoints.size() << " = " << numGridPoints << "\n";
    std::cout << "The number of points for given field = " << field.size() << "\n";
    exit(1);
  }

  if (fieldType == FieldType::Psi)
  {
    psiField = field;
    assert (psiField.size() == numGridPoints); 
  }
  else
  {
    std::cout << "The field of type = " << static_cast<int>(fieldType) << " is not supported yet\n";
    std::cout << "The list of supported fields are: ";
    std::cout << static_cast<int>(FieldType::Psi) << "\n";
    exit(1);
  }
}

// Function to set array of psi values from the magnetic field source file.
void GridFieldData::setPsiArray(const std::vector <double>& psi)
{
  psiArray = psi;
}

// Function to set an array of poloidal current values from the magnetic field source file.
void GridFieldData::setPoloidalCurrentArray(const std::vector <double>& poloidalCurrent)
{
  poloidalCurrentArray = poloidalCurrent;
}

// Function to set raw limiter data (wall curve) to the grid data.
void GridFieldData::setLimiter(const std::vector <double>& rLim, const std::vector <double>& zLim)
{
  rLimiterPoints = rLim;
  zLimiterPoints = zLim;
}

// Function to set plasma boundary (separatrix) data to the grid data.
void GridFieldData::setPlasmaBoundary(const std::vector <double>& rBdry, const std::vector <double>& zBdry)
{
  rBdryPoints = rBdry;
  zBdryPoints = zBdry;
}

// Function to set the domain box to grid data.
void GridFieldData::setDomainBox(const std::vector <double>& box)
{
  domainBox = box;
}

// Function to return r grid points.
const std::vector <double>& GridFieldData::getRPoints() const
{
  return rPoints;
}

// Function to return z grid points.
const std::vector <double>& GridFieldData::getZPoints() const
{
  return zPoints;
}

// Function to return a field of type double.
const std::vector <double>& GridFieldData::getDoubleFieldData(const FieldType& fieldType) const
{
  // Step 1: Check the field and return corresponding field vector.
  if (fieldType == FieldType::Psi)
  {
    // If field is not empty, return it. If empty, print an warning message.
    if (psiField.size())
      return psiField;
    else
    {
      std::cout << "Warning: psi Field is empty. Doesn't contain any data yet\n";
      return psiField;
    }
  }
  else
  {
    std::cout << "The field of type = " << static_cast<int>(fieldType) << " is not supported yet\n";
    std::cout << "The list of supported fields are: ";
    std::cout << static_cast<int>(FieldType::Psi) << "\n";
    exit(1);
  }  
}

// Function to return psi field array.
const std::vector <double>& GridFieldData::getPsiArray() const
{
  return psiArray;
}

// Function to return poloidal current field vector.
const std::vector <double>& GridFieldData::getPoloidalCurrentArray() const
{
  return poloidalCurrentArray;
}

// Function to return r coordinate of limiter points.
const std::vector <double>& GridFieldData::getLimiterPointsR() const
{
  return rLimiterPoints;
}

// Function to return z coordinate of limiter points.
const std::vector <double>& GridFieldData::getLimiterPointsZ() const
{
  return zLimiterPoints;
}

// Function to return r coordinate of physical boundary.
const std::vector <double>& GridFieldData::getBdryPointsR() const
{
  return rBdryPoints;
}

// Function to return z coordinate of physical boundary.
const std::vector <double>& GridFieldData::getBdryPointsZ() const
{
  return zBdryPoints;
}

// Function to return a vector of size 4 for the rectangular domain box.
const std::vector <double>& GridFieldData::getDomainBox() const
{
  return domainBox;
}
