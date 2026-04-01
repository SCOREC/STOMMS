#include <iostream>
#include <string>
#include <array>

#include "SimUtil.h"
#include "SimModel.h"
#include "MeshSim.h"

struct Inputs{
  std::string stommsMeshGen;
  std::string testDir;
};

Inputs verifyInputs(int argc, char** argv);
bool checkFileName(std::string s, std::string fileName);
int validateEntitiesCount(const pGModel& model, const pMesh& mesh, const std::string& testCase);
std::array<int,6> getEntityValidationData(const std::string& testCase);

// Main Test Function
int main(int argc, char** argv)
{ 
  // Step 1: Verify the inputs
  Inputs in = verifyInputs(argc, argv);

  // Step 2: Run the basic tomms_meshgen command to see if it works
  std::cout << "Test Directory = " << in.testDir << "\n";
  std::string runMeshGen = "cd \"" + in.testDir + "\" && \"" + in.stommsMeshGen + "\""; 
  int meshGenReturn = std::system(runMeshGen.c_str());
  if (meshGenReturn != 0)
  {
    std::cerr << "ERROR: stomms_meshgen exited with code " << meshGenReturn << "\n";
    exit(1);
  }

  // Step 3: Load model and mesh, and validate results
  MS_init();  // Intialize Simmetrix object

  // Step 3.1: Read model/mesh names
  std::string modelName = in.testDir + "/simModel.smd";
  std::string meshName = in.testDir + "/simMesh.sms";

  // Step 3.2: Load model/mesh
  pGModel model = GM_load(modelName.c_str(), nullptr, nullptr);
  pMesh mesh = M_load(meshName.c_str(), model, nullptr);

  // 3.3: Validate results
  std::cout << "Test Dir = " << in.testDir << "\n";
  int result = validateEntitiesCount(model, mesh, in.testDir);
  if (result != 0)
  {
    std::cerr << "ERROR: Model/Mesh validation failed. \n";    
    exit(1);
  }

  MS_exit();

  std::cout << "Test Passed\n";
  return 0;
}

Inputs verifyInputs(int argc, char** argv)
{
  Inputs inputs;
  
  // Step 1: Verify that we have required number of inputs
  if (argc < 3)
  {
    std::cerr << "Error: Inputs Missing\n";
    std::cerr << "Usage: ./test_exe stomms_meshgen test_case_directory\n";
    exit(1);
  }

  // Step 2: Verify path to tomms_meshgen is correct and if yes, save it.
  std::string stommsExe = argv[1]; 
  if (stommsExe.compare(stommsExe.length() - 14, 14, "stomms_meshgen") == 0)
    inputs.stommsMeshGen = stommsExe;
  else
  {
    std::cerr << "Error: stomms_meshgen is missing or path to it is incorrect\n";
    std::cerr << "Usage: ./test_exe stomms_meshgen test_case_directory\n";
    exit(1);
  }

  // Step 3: Read the test directory
  inputs.testDir = argv[2];
  
  return inputs;  
}

int validateEntitiesCount(const pGModel& model, const pMesh& mesh, const std::string& testCase)
{
  // Step 1: Read model. mesh entities count from Simmetrix model and mesh.
  int modelVertices, modelEdges, modelFaces, meshVertices, meshEdges, meshFaces;
  modelVertices = GM_numVertices(model);
  modelEdges = GM_numEdges(model);
  modelFaces = GM_numFaces(model);
  meshVertices = M_numVertices(mesh);
  meshEdges = M_numEdges(mesh);
  meshFaces = M_numFaces(mesh);
  
  // Step 2: Get validation data.
  std::array<int,6> validationData = getEntityValidationData(testCase);

  // Step 3: Test number of model entities
  if (modelVertices != validationData[0])
  {
    std::cout << "Error: Number of model vertices: " << modelVertices << "(Expected: " <<  validationData[0] << ")\n";
    return 1;
  }

  if (modelEdges != validationData[1])
  {
    std::cout << "Error: Number of model edges: " << modelEdges << "(Expected: " <<  validationData[1] << ")\n";
    return 1;
  }
  
  if (modelFaces != validationData[2])
  {
    std::cout << "Error: Number of model faces: " << modelFaces << "(Expected: " <<  validationData[2] << ")\n";
    return 1;
  }

  // Step 4: Test number of mesh entities(within tolerance).
  double tolerance = 0.01; // 1%  
  std::array<int,2> numVerticesBounds = {static_cast<int>((1-0.01)*validationData[3]), static_cast<int>((1+0.01)*validationData[3])};

  if (meshVertices < numVerticesBounds[0] || meshVertices > numVerticesBounds[1])
  {
    std::cout << "Error: Number of mesh vertices: " << meshVertices << "(Expected: " <<  validationData[3] << ")\n";
    return 1;
  }
  
  std::array<int,2> numEdgesBounds = {static_cast<int>((1-tolerance)*validationData[4]), static_cast<int>((1+tolerance)*validationData[4])};
  if (meshEdges < numEdgesBounds[0] || meshEdges > numEdgesBounds[1])
  {
    std::cout << "Error: Number of mesh edges: " << meshEdges << "(Expected: " <<  validationData[4] << ")\n";
    return 1;
  }

  std::array<int,2> numFacesBounds = {static_cast<int>((1-tolerance)*validationData[5]), static_cast<int>((1+tolerance)*validationData[5])};
  if (meshFaces < numFacesBounds[0] || meshFaces > numFacesBounds[1])
  {
    std::cout << "Error: Number of mesh faces: " << meshFaces << "(Expected: " <<  validationData[5] << ")\n";
    return 1;
  }

  return 0;
}

bool checkFileName(std::string s, std::string fileName) 
{
  if(s.substr(s.find_last_of("/") + 1) == fileName) 
    return true;
  else 
    return false;
}

std::array<int,6> getEntityValidationData(const std::string& testCase)
{
  std::array<int,6> data;
  if (checkFileName(testCase, "DIIID-g096333.03337"))
    data = std::array<int,6>{66, 72, 21, 2367, 6969, 4603};
  if (checkFileName(testCase, "KSTAR-g018451.002790_kin_1"))
    data = std::array<int,6>{317, 406, 203, 214792, 643014, 428223}; 
 
  // Data for production scale meshes

  return data;
}
