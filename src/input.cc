#include "input.h"

args::args(int argc, char* argv[])
{
  // First set the default values of parameters - write a fucntion for it
  setDefaultValues();  

  // Read the parameters from the input file
  setValuesFromInputFile();

  // Read the input values, and set them to local variables for further use.
  setValuesForLocalUse();

}

// This function initializes the input parameters with default values.
void args::setDefaultValues()
{
  // Values to read from users.
  // Nothing here yet.
  
  // Variables for internal use.
  vmecFileFound = false;
  eqdskFileFound = false;
}

// This function reads the input parameter from the mesh input file.
void args::setValuesFromInputFile()
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
      vmecFileFound = true;
    }
    else if (token == "eqdskFile")
    {
      input >> eqdskFile;
      eqdskFileFound = true;
    }
    else if (token == "fluxFile")
      input >> fluxFile;
    else if (token == "planeFile")
      input >> planeFile;
    else if (token == "meshSizeFile")
      input >> meshSizeFile;
  }
  input.close();
}

// A function to set values to local variables and containers for internal code use.
void args::setValuesForLocalUse()
{
  if (vmecFileFound)
  	in.vm = readVmecFile();	// Read the vmec file and store data in vmecData vm.
  if (eqdskFileFound)
	in.eq = readEqdskFile();
  in.fd.fluxInput = readFluxFile();  // Read the flux input file.
  in.pd.planeInput = readPlaneFile();  // Read the plane input file.
  in.fd.fluxMeshSize = readMeshSizeOnFlux();  // Read the mesh size input file.
  setPsiBounds();  // Set the psi values of axis and last closed flux curve.
}

// A function to set mesh sizes on each flux curves for later use (in meshing).
std::map<double,int> args::readMeshSizeOnFlux()
{
  int numFlux;  // Number of flux curves from first line of the file.
  double psiNorm;  // To read the normalized psi values of flux curves one by one from file.
  int numPoints;  // To read the number of desired points for each flux curve from file.
  std::map <double, int> meshSizeOnCurves; // A map between flux psi values and number of points.  
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
  while(meshInput >> psiNorm && meshInput >> numPoints)
  {
    if (psiNorm < 0.0 || psiNorm > 1.0)
    {
      std::cout << " The normalized psi value  =  " << psiNorm << " from mesh size input file will not be used since it was either lesser than 0.0 (axis) or greater than the 1.0 (last closed flux curve)\n";
      continue;
    }
    meshSizeOnCurves[psiNorm] = numPoints;
  }

  // DEBUG - Delete it later
  std::map <double, int>::iterator itr;
  for (itr = meshSizeOnCurves.begin(); itr != meshSizeOnCurves.end(); itr++)
    double normPsi = itr->first;

  // Step 4: Adjust the number of vertices on O-point if needed. O-point can only have one vertex.
  if (meshSizeOnCurves.begin()->second != 1)
  {
    std::cout << "The number of mesh vertices on O-point are = " << meshSizeOnCurves.begin()->second << "\n";
    std::cout << "Since the number of vertices on O-point cannot exceed 1, this is set to 1\n";
    meshSizeOnCurves[meshSizeOnCurves.begin()->first] = 1;  
  }

  return meshSizeOnCurves;
} 

// Read flux file and check its validity
std::vector<double> args::readFluxFile()
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
std::vector<double> args::readPlaneFile()
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

// Read input VMEC file and store relevant data in struct vmecData.
VmecData args::readVmecFile()
{
  VmecData v;  
 
  // Step 1: Read all the variables in vmec file
  NcFile vFile(vmecFile, NcFile::read); 

  // netCDF Variables. First four hold single values and the rest are arrays of data.
  NcVar vmecMinor, vmecMajor, vmecSurf, vmecMode, vmecR, vmecZ, vmecL, vmecXm, vmecXn, vmecPsi, vmecIota;    

  // Step 2: Read required variables from vmec file
  vmecMinor = vFile.getVar("Aminor_p");
  vmecMajor = vFile.getVar("Rmajor_p");
  vmecSurf = vFile.getVar("ns");
  vmecMode = vFile.getVar("mnmax");
  vmecR = vFile.getVar("rmnc");
  vmecZ = vFile.getVar("zmns");
  vmecL = vFile.getVar("lmns");
  vmecXm = vFile.getVar("xm");
  vmecXn = vFile.getVar("xn");
  vmecPsi = vFile.getVar("phi");
  vmecIota = vFile.getVar("iotaf");

  // Step 3: Assign netCDF variables to local variables and arrays
  vmecMinor.getVar(&v.minorR);
  vmecMajor.getVar(&v.majorR);
  vmecSurf.getVar(&v.nSurf);
  vmecMode.getVar(&v.nMode);

  v.R.resize(v.nSurf*v.nMode);
  v.Z.resize(v.nSurf*v.nMode);
  v.L.resize(v.nSurf*v.nMode);
  v.xm.resize(v.nMode);
  v.xn.resize(v.nMode);
  v.psi.resize(v.nSurf);
  v.iota.resize(v.nSurf);

  vmecR.getVar(v.R.data());
  vmecZ.getVar(v.Z.data());
  vmecL.getVar(v.L.data());
  vmecXm.getVar(v.xm.data());
  vmecXn.getVar(v.xn.data());
  vmecPsi.getVar(v.psi.data());
  vmecIota.getVar(v.iota.data());

  // Step 4: Return vmec data.
  return v;
}

EqdskData args::readEqdskFile()
{
  EqdskData gFileData;  // EqdskData struct to return

  // Step 1: Initialize few of the variables that are taken as
  // input in TOMMS (should be add to parametes list??)
  double eqd_psi_factor = 1.0;
  int reverse_psi = 0;

  // Step 2: Read the length of file string and set psi factor.
  // Not sure how factor works (but without it, it pspline throws an error.
  // Figure it out.
  int strLength = eqdskFile.length();
  set_eqd_psi_factor_(&eqd_psi_factor);

  // Step 3: Check if the file is eqd (no wall) or gFile,
  // and set the tag. Then laod the file.
  int eqd_tag = 0;

  // Step 3.1: If last 4 character of file name are ".eqd", its an eqd file.
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
  if(reverse_psi) rev = 1;

  // Step 5: Spline fitting of the discrete 2D data on background grid.
  init_ez_spline_(&rev, &eqd_tag); 
 
  return gFileData;
}

void args::setPsiBounds()
{
  psiAxis = in.vm.psi[0];
  psiLCF = in.vm.psi[in.vm.nSurf - 1];
}
