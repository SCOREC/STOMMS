#include <iostream>
#include "magneticGeometry.h"
#include "physicalGeometry.h"
#include "input.h"

// Structs and Helper Functions
struct Inputs{
  std::string testDir;  // Add more if needed
};

struct ValidationData{
  int numPlanes;
  std::vector <PhysicsPoint> oPoints;
  std::vector <PhysicsPoint> xPoints;
};

Inputs verifyInputs(int argc, char** argv);


// Main Test Function
int main(int argc, char** argv)
{
  // Step 1: Verify the inputs
  Inputs in = verifyInputs(argc, argv);

  // Step 2: Read the input file (mesh_input) for the input parameters
  args a(argc, argv);

  // Step 3: Setup the physical geometry (wall curve for now).
  //PhysicalGeometry physicalGeometry(a);

  // Step 4: Setup the magnetic field information. 
  //std::shared_ptr <MagneticGeometry> mg = setMagneticGeometry(a, physicalGeometry);

  //std::map<int, std::vector<PhysicsPoint>> oPoints = mg->getOPoints();
  //std::map<int, std::vector<PhysicsPoint>> xPoints = mg->getXPoints();
}

Inputs verifyInputs(int argc, char** argv)
{
  Inputs inputs;
  
  // Step 1: Verify that we have required number of inputs
  if (argc < 2)
  {
    std::cerr << "Error: Inputs Missing\n";
    std::cerr << "Usage: ./test_exe test_case_directory\n";
    exit(1);
  }

  // Step 2: Read the test directory
  inputs.testDir = argv[2];
  
  return inputs;  
}
