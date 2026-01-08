#include "input.h"

Inputs::Inputs()
{
  // First set the default values of parameters - write a fucntion for it
  setDefaultValues();  

  // Read the parameters from the input file
  setValuesFromInputFile();

  // Read the input values, and set them to local variables for further use.
  setValuesForLocalUse();

}

// This function initializes the input parameters with default values.
void Inputs::setDefaultValues()
{
  reactorType = ReactorType::Stellarator;  // Default Stellarator for now.
  reversePsi = false;
  eqdPsiFactor = 1.0;
  numPlanes = 64;  // default is 64
}

// This function reads the input parameter from the mesh input file.
void Inputs::setValuesFromInputFile()
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
      reactorType = ReactorType::Stellarator;
    }
    else if (token == "eqdskFile")
    {
      input >> eqdskFile;
      reactorType = ReactorType::Tokamak;
    }
    else if (token == "fluxFile")
      input >> fluxFile;
    else if (token == "planeFile")
      input >> planeFile;
    else if (token == "meshSizeFile")
      input >> meshSizeFile;
    else if (token == "limiterFile")
      input >> limiterFile;
    else if (token == "reversePsi")
      input >> reversePsi;
    else if (token == "eqdPsiFactor")
      input >> eqdPsiFactor;
  }
  input.close();
}

// A function to set values to local variables and containers for internal code use.
void Inputs::setValuesForLocalUse()
{
  // Step 1: Initialize magnetic field file sources if needed. 
  if (reactorType == ReactorType::Tokamak)
    initializeEqdskFile();

  // Step 2: Read files to set resolution.
  in.fd.fluxInput = readFluxFile();  // Read the flux input file.
  in.fd.fluxMeshSize = readMeshSizeOnFlux();  // Read the mesh size input file.

  if (reactorType == ReactorType::Stellarator)
    in.pd.planeInput = readPlaneFile();  // Read the plane input file.
  else if (reactorType == ReactorType::Tokamak)
    in.pd.planeInput.push_back(0.0); // 
}

// A function to set mesh sizes on each flux curves for later use (in meshing).
std::map<double, double> Inputs::readMeshSizeOnFlux()
{
  int numFlux;  // Number of flux curves from first line of the file.
  double psiNorm;  // To read the normalized psi values of flux curves one by one from file.
  double vertexSpacing;  // To read the desired mesh spacing for each flux curve from file.
  std::map <double, double> meshSizeOnCurves; // A map between flux psi values and number of points.  
  std::ifstream meshInput(meshSizeFile);

  // Step 1: If can't open the file, exit the program with an error message.
  if (!meshInput.is_open())
  {
    std::cout << "Error opening the mesh size input file " << meshSizeFile << "\n";
    exit(1);
  }
  
  // Step 2: If the first line is empty, exit with an error message. Else, the value
  // from the first line is stored to numFlux.
  if (!(meshInput >> numFlux))
  {
    std::cout << "The given file for mesh input seems to be empty" << "\n";
    exit(1);
  }

  // Step 3: Read the values from the file and store them in map.
  // Step 4: Make sure there is no flux with normalzied psi lesser than 0 or greater than 1.
  while(meshInput >> psiNorm && meshInput >> vertexSpacing)
  {
    if (psiNorm < 0.0 || psiNorm > 1.0)
    {
      std::cout << " The normalized psi value  =  " << psiNorm << " from mesh size input file will not be used since it was either lesser than 0.0 (axis) or greater than the 1.0 (last closed flux curve)\n";
      continue;
    }
    meshSizeOnCurves[psiNorm] = vertexSpacing;
  }

  return meshSizeOnCurves;
} 

// Read flux file and check its validity
std::vector<double> Inputs::readFluxFile()
{
  int numFlux;  // Number of flux curves from the first line of the file.
  double psiNorm;  // To read the normalized psi values of flux curves one by one from the file
  std::vector <double> psiNormVec;  // A vector storing the normalized psi values from the file.
  std::ifstream fluxInput(fluxFile);  // Load the file.

  // Step 1: If can't open the file, exit the program with an error message.
  if (!fluxInput.is_open())
  {
    std::cout << "Error opening the flux input file " << fluxFile << "\n";
    exit(1);
  }
  
  // Step 2: If the first line is empty, exit with an error message. Else, the value
  // from the first line is stored to numFlux
  if (!(fluxInput >> numFlux))
  {
    std::cout << "The given file for flux curves seems to be empty" << "\n";
    exit(1);
  }
  
  // Step 3: Read the values from the file and store them in psiNormVec vector.
  while(fluxInput >> psiNorm)
    psiNormVec.push_back(psiNorm);

  // Step 4: Make sure the given data is consistent.
  assert (numFlux == psiNormVec.size());

  // Step 5: Make sure there is no flux with normalzied psi lesser than 0 or greater than 1.
  for (int i = 0; i < psiNormVec.size(); i++)
  {
    double normPsi = psiNormVec[i];
    if (normPsi < 0.0 || normPsi > 1.0)
    {   
        psiNormVec.erase(psiNormVec.begin()+i);
        std::cout << " The normalized psi value =  " << normPsi << " is removed since it was either lesser than 0.0 (axis) or greater than the 1.0 (last closed flux curve)\n";
        i--;  // Makes sure to iterate over the element next to the deleted element.
    }   
  }

  // Step 6: Returns the normalized psi values vector.
  return psiNormVec;
}

// Read the planes file and check its validity.
// Converts the given angles in degrees to radians.
std::vector<double> Inputs::readPlaneFile()
{
  int numPlanes;  // Number of poloidal planes from the first line of the file.
  double angle;  // To read the poloidal plane angles one by one from the file.
  std::vector <double> planeAngles;  // A vector storing the plane angles in degrees from the file.
  std::ifstream planeInput(planeFile);  //Load the file.

  // Step 1: If can't open the file, exit the program with an error message.
  if (!planeInput.is_open())
  {
    std::cout << "Error opening the plane input file " << planeFile << "\n";
    exit(1);
  }
  
  // Step 2: If the first line is empty, exit with an error message. Else, the value
  // from the first line is stored to numPlanes
  if (!(planeInput >> numPlanes))
  {
    std::cout << "The given file for planes seems to be empty" << "\n";
    exit(1);
  }

  // Step 3: Read the values from the file and store them in planeAngles vector.
  while(planeInput >> angle)
    planeAngles.push_back(angle);

  // Step 4: Make sure the given data is consistent.
  assert (numPlanes == planeAngles.size());


  // Step 5: Make sure there is no plane angles lesser than 0 or greater than or equal to 360.
  std::vector <double> planeAnglesRadian;
  for (int i = 0; i < planeAngles.size(); i++)
  {
    double toroidalAngle = planeAngles[i];
    if (toroidalAngle < 0.0 || toroidalAngle > 360.0)
    {
        planeAngles.erase(planeAngles.begin()+i);
        std::cout << " The toroidal angle " << toroidalAngle << " is removed since it was either lesser than 0 or greater than 360\n";
        i--;  // Makes sure to iterate over the element next to the deleted element.
    }
    else
    {
      // Step 6; Convert the angles from degress to radians.
      double angleInRadian = toroidalAngle*(M_PI/180);
      planeAnglesRadian.push_back(angleInRadian);
    }
  }

  // Step 7: Return the vector containing the angles in radians.
  return planeAnglesRadian;
}

// Read input tokamak equilibrium file (.eqd or geqdsk).
// Need to initialize it here, so we can use PSPLINE
// functions in next steps. 
void Inputs::initializeEqdskFile()
{
  // Step 1: Read the length of file string and set psi factor.
  // Not sure how factor works (but without it, it pspline throws an error.
  // Figure it out.
  int strLength = eqdskFile.length();
  set_eqd_psi_factor_(&eqdPsiFactor);

  // Step 2: Check if the file is eqd (no wall) or gFile,
  // and set the tag. Then laod the file.
  int eqd_tag = 0;

  // Step 2.1: If last 4 character of file name are ".eqd", its an eqd file.
  if(eqdskFile.compare(eqdskFile.size()-4,4,".eqd")==0)
  {  // eqd format
    eqd_tag=1;
    readeqdfile_(eqdskFile.c_str(), &strLength);
  }
  else // else, its a gFile
  { // efit data file
    eqd_tag=0;
    readgfile_(eqdskFile.c_str(), &strLength);
  }

  // Step 4: If reverse psi, flip the signs of field.
  int rev = 0;
  if(reversePsi) rev = 1;

  // Step 5: Spline fitting of the discrete 2D data on background grid.
  init_ez_spline_(&rev, &eqd_tag);
}

// Function to return limiter (wall curve) file name.
const std::string& Inputs::getLimiterFile() const
{
  return limiterFile;
}

// Function to return reactor type (Stellarator, Tokamak).
const ReactorType& Inputs::getReactorType() const
{
  return reactorType;
}

// Function to VMEC file name.
const std::string& Inputs::getVmecFile() const
{
  return vmecFile;
}

// Function to return InputData struct which contains flux and planes info.
const InputData& Inputs::getInputData() const
{
  return in;
}

// Function to check if psi is reverse or not.
const bool& Inputs::useReversePsi() const
{
  return reversePsi;
}

// Returns the number of tokamak planes for magnetic field line tracing.
const int& Inputs::getNumTokamakPlanes() const
{
  return numPlanes;
}
