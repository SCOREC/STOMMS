#include "input.h"

args::args(int argc, char* argv[])
{
  // First set the default values of parameters - write a fucntion for it
  // void function_default_values();
  // Read the parameters from the input file
  set_values_from_input_file();

}

void args::set_values_from_input_file()
{

  inputFile = "mesh_input";
  std::ifstream input(inputFile);
  if (!input.is_open())
  {
    std::cout << "Error opening the input file " <<  inputFile << "\n";
    exit(1);	// If input file cannot be found, exit the program
  }

  std::string token;
  while (input >> token)
  {
    if (token == "vmecFile")
    {
      input >> vmecFile;
      std::cout << "The loaded VMEC file is " << vmecFile << "\n";
    }
    if (token == "fluxFile")
    {
      input >> fluxFile;
      std::cout << "The loaded flux indices file for the core region is " << fluxFile << "\n";
    }
    if (token == "planeFile")
    {
      input >> planeFile;
      std::cout << "The loaded planes position file for the placement of planes is " << planeFile << "\n"; 
    }
  }
  input.close();
}
