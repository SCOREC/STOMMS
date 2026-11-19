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
    data = std::array<int,6>{189, 213, 113, 34433, 103017, 68585};
  if (checkFileName(testCase, "d161218n_LH_D"))
    data = std::array<int,6>{358, 445, 227, 236848, 709543, 472696};
  if (checkFileName(testCase, "d3d_144981-03175_meshing_v2"))
    data = std::array<int,6>{294, 355, 179, 220492, 660478, 439987};
  if (checkFileName(testCase, "d3d_163240"))
    data = std::array<int,6>{244, 296, 137, 74109, 221390, 147282};
  if (checkFileName(testCase, "D-g157308.04200")) 
    data = std::array<int,6>{204, 234, 120, 96992, 290511, 193520};
  if (checkFileName(testCase, "ITER_10MA_xgca"))
    data = std::array<int,6>{522, 656, 335, 272363, 816054, 543692};
  if (checkFileName(testCase, "kstar_018451_2790_xgc1"))
    data = std::array<int,6>{317, 406, 203, 215327, 644552, 429226};
  if (checkFileName(testCase, "kstar_018451_2790_xgca"))
    data = std::array<int,6>{304, 390, 193, 50183, 150030, 99848};
  if (checkFileName(testCase, "LTX_1504291255_47400"))
    data = std::array<int,6>{44, 43, 43, 6539, 19257, 12719};
  if (checkFileName(testCase, "ltx_1504291255_47400_xgc1"))
    data = std::array<int,6>{278, 277, 86, 33397, 99555, 66159};
  if (checkFileName(testCase, "ltx_1504291255_47400_xgca"))
    data = std::array<int,6>{260, 271, 56, 15908, 47227, 31320};
  if (checkFileName(testCase, "nstx_132588_00650_v2"))
    data = std::array<int,6>{149, 179, 89, 131266, 392737, 261472};
  if (checkFileName(testCase, "nstx_132588_development_sample"))
    data = std::array<int,6>{93, 98, 59, 241474, 723470, 481997};
  if (checkFileName(testCase, "nstx_132588_em_20210726"))
    data = std::array<int,6>{115, 136, 64, 81907, 244740, 162834};
  if (checkFileName(testCase, "nstx_132588_v3"))
    data = std::array<int,6>{122, 141, 73, 289206, 866360, 577155};
  if (checkFileName(testCase, "nstx_132588_v4"))
    data = std::array<int,6>{121, 140, 73, 256563, 768518, 511956};
  
  // Data for production scale meshes
  if (checkFileName(testCase, "ITER_10MA"))
    data = std::array<int,6>{713, 963, 412, 1433311, 4294617, 2861307};
  if (checkFileName(testCase, "ITER_15MA_H_EM_20210922"))
    data = std::array<int,6>{628, 761, 429, 1763269, 5287847, 3524579};

  return data;
}
