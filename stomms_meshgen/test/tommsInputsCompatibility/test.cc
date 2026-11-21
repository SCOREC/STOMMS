#include <iostream>
#include <string>
#include <array>

#include "SimUtil.h"
#include "SimModel.h"
#include "MeshSim.h"

struct Inputs{
  std::string tommsMeshGen;
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

  // Step 2: Run the basic stomms_meshgen command to see if it works
  std::cout << "Test Directory = " << in.testDir << "\n";
  std::string runMeshGen = "cd \"" + in.testDir + "\" && \"" + in.tommsMeshGen + "\""; 
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

  // Step 2: Verify path to stomms_meshgen is correct and if yes, save it.
  std::string tommsExe = argv[1]; 
  if (tommsExe.compare(tommsExe.length() - 14, 14, "stomms_meshgen") == 0)
    inputs.tommsMeshGen = tommsExe;
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
  if (checkFileName(testCase, "coarse"))
    data = std::array<int,6>{195, 219, 113, 34437, 103025, 68589};
  if (checkFileName(testCase, "d161218n_LH_D"))
    data = std::array<int,6>{364, 451, 227, 236821, 709461, 472641};
  if (checkFileName(testCase, "d3d_144981-03175_meshing_v2"))
    data = std::array<int,6>{300, 361, 179, 220558, 660674, 440117};
  if (checkFileName(testCase, "d3d_163240"))
    data = std::array<int,6>{255, 307, 137, 73895, 220765, 146871};
  if (checkFileName(testCase, "D-g157308.04200")) 
    data = std::array<int,6>{210, 240, 120, 96654, 289518, 192865};
  if (checkFileName(testCase, "ITER_10MA_xgca"))
    data = std::array<int,6>{522, 656, 335, 272331, 815967, 543637};
  if (checkFileName(testCase, "kstar_018451_2790_xgc1"))
    data = std::array<int,6>{317, 406, 203, 214788, 643006, 428219};
  if (checkFileName(testCase, "kstar_018451_2790_xgca"))
    data = std::array<int,6>{304, 390, 193, 50152, 149950, 99799};
  if (checkFileName(testCase, "LTX_1504291255_47400"))
    data = std::array<int,6>{44, 43, 43, 6539, 19257, 12719};
  if (checkFileName(testCase, "ltx_1504291255_47400_xgc1"))
    data = std::array<int,6>{278, 277, 86, 33440, 99684, 66245};
  if (checkFileName(testCase, "ltx_1504291255_47400_xgca"))
    data = std::array<int,6>{260, 271, 56, 15799, 46905, 31107};
  if (checkFileName(testCase, "nstx_132588_00650_v2"))
    data = std::array<int,6>{149, 179, 89, 131349, 392982, 261634};
  if (checkFileName(testCase, "nstx_132588_development_sample"))
    data = std::array<int,6>{93, 98, 59, 239596, 717833, 478238};
  if (checkFileName(testCase, "nstx_132588_em_20210726"))
    data = std::array<int,6>{115, 136, 64, 81909, 244746, 162838};
  if (checkFileName(testCase, "nstx_132588_v3"))
    data = std::array<int,6>{122, 141, 73, 285556, 855422, 569867};
  if (checkFileName(testCase, "nstx_132588_v4"))
    data = std::array<int,6>{121, 140, 73, 254633, 762754, 508122};
  
  // Data for production scale meshes
  if (checkFileName(testCase, "ITER_10MA"))
    data = std::array<int,6>{713, 963, 412, 1433311, 4294617, 2861307};
  if (checkFileName(testCase, "ITER_15MA_H_EM_20210922"))
    data = std::array<int,6>{628, 761, 429, 1763269, 5287847, 3524579};

  return data;
}
