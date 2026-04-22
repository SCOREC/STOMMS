#include <iostream>
#include "magneticGeometry.h"
#include "physicalGeometry.h"
#include "input.h"
#include "stomms.h"

// Structs and Helper Functions
struct Input{
  std::string testName;  // Add more if needed
};

struct ValidationData{
  int numPlanes;
  std::vector <PhysicsPoint> oPoints;
  std::vector <PhysicsPoint> xPoints;
};

Input verifyInputs(int argc, char** argv);
ValidationData getValidationData(const std::string& testCase);
bool validateData(const ValidationData& data, const std::map<int, std::vector<PhysicsPoint>>& oPoints,
                  const std::map<int, std::vector<PhysicsPoint>>& xPoints);

// Main Test Function
int main(int argc, char** argv)
{
  // Step 1: Verify the inputs
  Input in = verifyInputs(argc, argv);

  // Step 2: Read the input file (mesh_input) for the input parameters.
  Inputs input;
  
  // Step 3: Setup the physical geometry (wall curve for now).
  PhysicalGeometry physicalGeometry(input);

  // Step 4: Setup the model metadata by setting up the meta data on poloidal planes.
  ModelMetaData modelMetaData(input);

  // Step 5: Get the oPoints and xPoints.
  WallCurve wall = physicalGeometry.getWallCurveAtPlane(0);
  bool reversePsi = input.useReversePsi();
  CriticalPointsEqdsk criticalPoints(wall, reversePsi);
  std::vector <PhysicsPoint> oPointsVec = criticalPoints.getOPoints();
  std::vector <PhysicsPoint> xPointsVec = criticalPoints.getXPoints();
  std::sort(oPointsVec.begin(), oPointsVec.end(), comparePhysicsPoints);
  std::sort(xPointsVec.begin(), xPointsVec.end(), comparePhysicsPoints);
    
  // Step 6: Setup  them in terms of planes map (only one for now)
  std::map<int, std::vector<PhysicsPoint>> oPoints, xPoints;
  oPoints[0] = oPointsVec;
  xPoints[0] = xPointsVec; 

  // Step 7: Get validation data
  ValidationData data = getValidationData(in.testName);

  // Step 8: Compare validation data with the critical points data.
  if (!validateData(data, oPoints, xPoints))
    return 1;  

  return 0;
}

Input verifyInputs(int argc, char** argv)
{
  Input inputs;
  
  // Step 1: Verify that we have required number of inputs
  if (argc < 2)
  {
    std::cerr << "Error: Inputs Missing\n";
    std::cerr << "Usage: ./test_exe test_case_directory\n";
    exit(1);
  }

  // Step 2: Read the test directory
  inputs.testName = argv[1];
  
  return inputs;  
}

bool arePhysicsPointsSame(const PhysicsPoint& pt1, const PhysicsPoint& pt2)
{
  std::cout << "Pt1 = " << pt1.getPoint().x << " , " << pt1.getPoint().y << " , psi = " << pt1.getPsi() << "\n";
  double diffXCoord = fabs(pt1.getPoint().x - pt2.getPoint().x);
  double diffYCoord = fabs(pt1.getPoint().y - pt2.getPoint().y);
  double diffPsi = fabs(pt1.getPsi() - pt2.getPsi());

  double tolerance = 1e-3;
  if (diffXCoord <= tolerance && diffYCoord <= tolerance && diffPsi <= tolerance)
    return true;

  return false;
}

bool validateData(const ValidationData& data, const std::map<int, std::vector<PhysicsPoint>>& oPoints,
                  const std::map<int, std::vector<PhysicsPoint>>& xPoints)
{
  // Step 1: Validate number of planes.
  if (data.numPlanes != oPoints.size())
    return false;
  
  // Step 2: Validate oPoints
  for (auto it = oPoints.begin(); it != oPoints.end(); it++)
  {
    std::vector <PhysicsPoint> oPts = it->second;
    for (int i = 0; i < oPts.size(); i++)
    {
      PhysicsPoint pt1 = oPts[i];
      PhysicsPoint pt2 = data.oPoints[i];
      if (!arePhysicsPointsSame(pt1, pt2))
      {
        std::cout << "Opoint # " << i << " doesn't match the validation data\n";
        return false;
      }
    }
  }

  // Step 3: Validate xPoints
  for (auto it = xPoints.begin(); it != xPoints.end(); it++)
  {
    std::vector <PhysicsPoint> xPts = it->second;
    for (int i = 0; i < xPts.size(); i++)
    {
      PhysicsPoint pt1 = xPts[i];
      PhysicsPoint pt2 = data.xPoints[i];
      if (!arePhysicsPointsSame(pt1, pt2))
      {
        std::cout << "Xpoint # " << i << " doesn't match the validation data\n";
        return false;
      }
    }
  }

  return true;
} 

bool checkFileName(std::string s, std::string fileName) 
{
  if(s.substr(s.find_last_of("/") + 1) == fileName) 
    return true;
  else 
    return false;
}

ValidationData getValidationData(const std::string& testCase)
{
  ValidationData data;
  if (checkFileName(testCase, "DIIID-g096333.03337"))
  {
    data.numPlanes = 1;
    Point oPt1Coord(1.72485, 0.020566);
    PhysicsPoint oPt1(oPt1Coord, -0.209239, PhysicsPointType::OPoint);
    Point xPt1Coord(1.55764, -1.17717);
    PhysicsPoint xPt1(xPt1Coord, 0.136817, PhysicsPointType::XPoint);
    data.oPoints.push_back(oPt1);
    data.xPoints.push_back(xPt1);
  }
  else if (checkFileName(testCase, "ITER-10MA.geqdsk-xgca"))
  {
    data.numPlanes = 1;
    Point oPt1Coord(6.55989, 0.573433);
    PhysicsPoint oPt1(oPt1Coord, -10.5537, PointType::OPoint);
    Point xPt1Coord(5.11698, -3.42392);
    PhysicsPoint xPt1(xPt1Coord, -1.50227, PointType::XPoint);
    data.oPoints.push_back(oPt1);
    data.xPoints.push_back(xPt1);
  }
  else if(checkFileName(testCase, "KSTAR-g018451.002790_kin_1"))
  {
    data.numPlanes = 1;
    Point oPt1Coord(1.82705, -0.0370913);
    PhysicsPoint oPt1(oPt1Coord, -0.237975, PhysicsPointType::OPoint);
    Point xPt1Coord(1.45405, -0.879433);
    PhysicsPoint xPt1(xPt1Coord, -0.0859732, PhysicsPointType::XPoint);
    Point xPt2Coord(1.4471, 0.866713);
    PhysicsPoint xPt2(xPt2Coord, -0.0719775, PhysicsPointType::XPoint);

    data.oPoints.push_back(oPt1);
    data.xPoints.push_back(xPt1);
    data.xPoints.push_back(xPt2);
  }
  else if(checkFileName(testCase, "LTX-1504291255_47400.eqdsk"))
  {
    data.numPlanes = 1;
    Point oPt1Coord(0.41723, -0.0149901);
    PhysicsPoint oPt1(oPt1Coord, -0.0032656, PhysicsPointType::OPoint);

    data.oPoints.push_back(oPt1);
  } 
  else if(checkFileName(testCase, "MASTU-g_p45152_t0.80000"))
  {
    data.numPlanes = 1;
    Point oPt1Coord(1.00421, -0.0117952);
    PhysicsPoint oPt1(oPt1Coord, -0.0293902, PhysicsPointType::OPoint);
    Point xPt1Coord(0.608069, -1.13799);
    PhysicsPoint xPt1(xPt1Coord, 0.0840873, PhysicsPointType::XPoint);
    Point xPt2Coord(0.60643, 1.13325);
    PhysicsPoint xPt2(xPt2Coord, 0.0850264, PhysicsPointType::XPoint);
    Point xPt3Coord(1.40879, -1.6701);
    PhysicsPoint xPt3(xPt3Coord, 0.0948984, PhysicsPointType::XPoint);
    Point xPt4Coord(1.41301, 1.66151);
    PhysicsPoint xPt4(xPt4Coord, 0.0950945, PhysicsPointType::XPoint);

    data.oPoints.push_back(oPt1);
    data.xPoints.push_back(xPt1);
    data.xPoints.push_back(xPt2);
    data.xPoints.push_back(xPt3);
    data.xPoints.push_back(xPt4);
  } 
  else if(checkFileName(testCase, "NSTX-g132588.00650"))
  {
    data.numPlanes = 1;
    Point oPt1Coord(1.03567, -0.0826343);
    PhysicsPoint oPt1(oPt1Coord, -0.0474163, PointType::OPoint);
    Point xPt1Coord(0.376017, -1.45911);
    PhysicsPoint xPt1(xPt1Coord, 0.0123977, PointType::XPoint);
    Point xPt2Coord(0.378819, 1.29527);
    PhysicsPoint xPt2(xPt2Coord, 0.0165915, PointType::XPoint);

    data.oPoints.push_back(oPt1);
    data.xPoints.push_back(xPt1);
    data.xPoints.push_back(xPt2);
  } 
   
  return data;
}
