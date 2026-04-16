#include "magneticGeometryTokamak.h"

/***********************************************/
// Class: MagneticGeometryForTokamak
// Derived class for MagneticGeometry
/***********************************************/
MagneticGeometryForTokamak::MagneticGeometryForTokamak(const ModelMetaData& modelMetaData, const WallCurve& wall, const Inputs& input)
{
  // Step 1: Find critical points from EQDSK file and print them.
  wallCurve = wall;
  reversePsi = input.useReversePsi();
  CriticalPointsEqdsk criticalPoints(wall, reversePsi);
  planeMetaData = modelMetaData.getPlaneMetaDataByIndex(0);
  std::cout << "Reverse Psi " << (reversePsi == true ? "ON" : "OFF" ) << "\n";

  // Step 2: Read and sort critical points.
  std::vector <PhysicsPoint> oPointsVec = criticalPoints.getOPoints();
  std::vector <PhysicsPoint> xPointsVec = criticalPoints.getXPoints();
  std::sort(oPointsVec.begin(), oPointsVec.end(), comparePhysicsPoints);
  std::sort(xPointsVec.begin(), xPointsVec.end(), comparePhysicsPoints);

  // Step 3: Print critical points
  printCriticalPoints(oPointsVec);
  printCriticalPoints(xPointsVec);

  // Step 4: Setup critical points on each plane.
  // Just one plane for tokamak
  oPoints[0] = oPointsVec;
  xPoints[0] = xPointsVec;

  // Step 5: Setup psi values into types (open, closed, separatrix etc.)
  std::vector<PlaneMetaData> planesContainer = modelMetaData.getPlanesContainer();
  psiNormList = planesContainer[0].getPlaneFluxValues();
  classifyPsiValues(input);
  
  // Step 6: Set up Eqdsk Data class for curve generation.
  EqdskData eqdskData(input, planeMetaData, oPointsVec[0], psiCoreBoundary);
  genFluxCurves(planeMetaData, eqdskData, wallCurve);

  // Step 7: Populate CurveContainer with flux curves info
  CurveContainer curvesContainer(closedCurves, separatrixCurves, wallCurve);
  curvesContainer.setCriticalPoints(oPointsVec, xPointsVec);

  // Step 8: Generate the model
  modelEqdsk = ModelEqdsk(planeMetaData, eqdskData, curvesContainer); 
}

// Classify psi normalized values into respective types (open, closed etc.)
void MagneticGeometryForTokamak::classifyPsiValues(const Inputs& in)
{
  std::cout << "\n========== FLUX CURVES CLASSIFICATION ==========\n";
  // Step 1: Setup psi of axis point. Since Tokamak has one plane so opoints
  // at zeroth plane, and first entry of oPoints. For cases with mutliple
  // opoints, we might need to define a logic to find axis point in future.
  psiAxis = oPoints[0][0].getPsi();  

  // Step 2: Get psi for separatrix to get psi from normalized value. Also, to
  // set all psi values for separatrix.
  std::vector <PhysicsPoint> xPts = xPoints[0];
  for (int i = 0; i < xPts.size(); i++)
    psiValuesSeparatrix.push_back(xPts[i].getPsi());

  if (xPts.size())
    psiCoreBoundary = psiValuesSeparatrix[0];
  else // Zero xPoint case
    psiCoreBoundary = in.getLastClosedPsi();
 
  std::cout << "Psi Boundary = " << psiCoreBoundary << "\n";

  // First value = 0.0 belongs to psiAxis so starts from second member of vector.
  bool psiSep = false;
  for (int i = 1; i < psiNormList.size(); i++)  
  {
    double psiNorm = psiNormList[i];
    double psi = convertNormToPsi(psiNorm, psiAxis, psiCoreBoundary);
    if ((!xPts.size() && psiNorm <=1) || (xPts.size() && psiNorm < 1))
    {
      psiValuesClosed.push_back(psi);
      std::cout << "Closed Curves ||  psiNorm = " << psiNorm << " , psi = " << psi << "\n";
    }
    if (psiNorm > 1)
    {
      for (int j = 1; j < psiValuesSeparatrix.size(); j++)
      {
        if (fabs(psi - psiValuesSeparatrix[j]))
        {
          psiSep = true;
          break;
        }
      }
      if (psiSep)
        continue;

      psiValuesOpen.push_back(psi);
      std::cout << "Open Curves ||  psiNorm = " << psiNorm << " , psi = " << psi << "\n";
    }
  }

  for (int i = 0; i < psiValuesSeparatrix.size(); i++)
    std::cout << "Separatrix Curves || psi = " << psiValuesSeparatrix[i] << "\n";
}

void MagneticGeometryForTokamak::genFluxCurves(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, const WallCurve& wall)
{
 std::cout << "\n========== FLUX CURVES GENERATION ==========\n";

  std::cout << ".......... Generating Closed Curves\n";
  closedCurves = genClosedFluxCurves(psiValuesClosed, oPoints[0][0], eqdskData, planeMetaData);

  std::cout << ".......... Generating Separatrices\n";
  separatrixCurves = genSeparatrixCurves(xPoints.at(0), eqdskData, wallCurve, planeMetaData);

  std::cout << ".......... Separatrix & Closed Curves: DONE\n";
}

// Function to get a map between plane number and vector of OPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryForTokamak::getOPoints() const
{
  return oPoints;
}

// Function to get a map between plane number and vector of XPoints.
const std::map<int, std::vector<PhysicsPoint>>& MagneticGeometryForTokamak::getXPoints() const
{
  return xPoints;
}

// A function to return psi value of the axis in the tokamak domain.
double MagneticGeometryForTokamak::getPsiAxis() const
{
  PhysicsPoint axis = oPoints.at(0).at(0);  // first member on first plane. 
  return axis.getPsi();
}

// A function to return psi value of the innermost separatrix.
double MagneticGeometryForTokamak::getPsiCoreBoundary() const
{
  PhysicsPoint xPt = xPoints.at(0).at(0);
  return xPt.getPsi();
}

// Function to get model associated with tokamak geometry.
const Model& MagneticGeometryForTokamak::getModel() const
{
  return modelEqdsk.getModel();
}

// Function to get all the geometric information on individual planes.
const std::vector <Plane>& MagneticGeometryForTokamak::getPlanes() const
{
  return modelEqdsk.getPlanes();
}
