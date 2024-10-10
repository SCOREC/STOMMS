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
  fieldDataFileOn = 0;
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
    if (token == "meshSizeFile")
    {
      input >> meshSizeFile;
      std::cout << "The loaded file for the mesh sizes on flux curves is " << meshSizeFile << "\n";
    }
    if (token == "fieldDataFileOn")
      input >> fieldDataFileOn;
    if (token == "fieldDataFile")
    {
      input >> fieldDataFile;
      if (fieldDataFileOn == 1)
        std::cout << "The loaded file for the field line data is " << fieldDataFile << "\n";
    }
  }
  input.close();
}

// A function to set values to local variables and containers for internal code use.
void args::setValuesForLocalUse()
{
  in.vm = readVmecFile();	// Read the vmec file and store data in vmecData vm.
  in.fd.fluxInput = readFluxFile();  // Read the flux input file.
  in.pd.planeInput = readPlaneFile();  // Read the plane input file.
  in.fd.fluxMeshSize = readMeshSizeOnFlux();  // Read the mesh size input file.
  if (fieldDataFileOn == 1)
    in.fieldData = readFieldLinePoints();  // Read the field line data file for point placement.
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
    if (toroidalAngle < 0.0 || toroidalAngle >= 360.0)
    {
        planeAngles.erase(planeAngles.begin()+i);
        std::cout << " The toroidal angles " << toroidalAngle << " is removed since it was either lesser than 0 or greater than or equal to 360\n";
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
  NcVar vmecMinor, vmecMajor, vmecSurf, vmecMode, vmecR, vmecZ, vmecL, vmecXm, vmecXn, vmecPsi;    

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

  vmecR.getVar(v.R.data());
  vmecZ.getVar(v.Z.data());
  vmecL.getVar(v.L.data());
  vmecXm.getVar(v.xm.data());
  vmecXn.getVar(v.xn.data());
  vmecPsi.getVar(v.psi.data());

  // Step 4: Return vmec data.
  return v;
}

// Read the input data for field lines and sort it for according to each flux on each plane.
std::vector <PlaneFieldLineData> args::readFieldLinePoints()
{

  // Step 1: Read the input planes and flux curves from already read data (fd and pd). 
  int numFlux = in.fd.fluxInput.size();
  int numPlanes = in.pd.planeInput.size();

  // Step 2: Read normalized psi values (psiN) and number of points for each flux curve (numP)
  // and put them in vectors psiNormal and numPoints respectively. Also, read xyz of each file 
  // line to xVec, yVec, and zVec respectively from the input field data file.
  double psiN;
  int numP;
  std::ifstream fieldInput(fieldDataFile);
  std::vector <double> xVec, yVec, zVec, psiNormal; 
  std::vector <int> numPoints; 

  // Step 2.1: Iterate over each flux curve (numFlux -1 because we are not readin O-point data,
  // its just a point and not defined by field line).
  for (int i = 0; i < numFlux -1; i++)
  {
    // Step 2.2: Read the values corresponding to each flux curve and put them in corresponding vectors.
    fieldInput >> psiN;
    fieldInput >> numP;
    psiNormal.push_back(psiN);
    numPoints.push_back(numP);

    // Step 2.3: For each flux curve, we will have total points = numplanes*numPoints on each flux curve.
    // Read those points and store them in xVec, yVec,zVec vectors. 
    double x,y,z;
    for (int j = 0; j < numP*numPlanes; j++)
    {
      fieldInput >> x;
      fieldInput >> y;
      fieldInput >> z;
      xVec.push_back(x);
      yVec.push_back(y);
      zVec.push_back(z);
    }
  }

  // Step 3: The data stored above is according to field line on each flux curve.
  // (like f0: p0,p1,..,pn, f1:p0,p1,..,pn, ...).
  // We need to sort it for flux curves on individual planes (like p0: f0,f1,..,fn,
  // p1: f0,f1,..,fn). Since we will eventaually loop over planes to access data and attributes.
  // Store the sorted data in vector of maps between flux value (normalized) and vector of points.
  // The vector size == # of plane. Each entity in vector holds the data for one plane.
  std::vector <PlaneFieldLineData> fieldData;

  // Step 3.1: Iterate over each plane and set data to it.
  for (int i = 0; i < numPlanes; i++)
  {
    // Step 3.2: Initialize the object PlaneFieldData to hold flux data map
    // on each plane. Read the data fieldMap for each plane and then set it
    // to pF.
    PlaneFieldLineData pF;
    std::map<double, std::vector <Pt>> fieldMap;

    // Step 3.3: Need to store # of points on the last used flux curve (numLast) to 
    // rebase the loop for each flux curve reading. 
    int numLast = 0;
   
    // Step 3.4: Iterate over the flux curves.
    for (int j = 0; j < numFlux-1; j++)
    {
      // Step 3.4.1: Read number of points on each flux curves and store them in fieldPoints
      // vector by iterating over the number of points.
      int numPt = numPoints[j];  
      std::vector <Pt> fieldPoints;
      for (int k = 0; k < numPt-1; k++)  // numPt -1: since last and first points are same.
      {
        // Step 3.4.2: Read point one by one.
        Pt point;
        point.x = xVec[(k*numPlanes)+(numPlanes*numLast)+i];

        // Note: There is an inconssitency with the sign of z-ccordinate except for plane 0.
        // Need to figure it out. For now, switch the signs of z for planes higher than 0.
        if (i == 0)
        {
          point.y = 0.0;
          point.z = zVec[(k*numPlanes)+(numPlanes*numLast)+i];
        }
        point.y = yVec[(k*numPlanes)+(numPlanes*numLast)+i];
        point.z = -zVec[(k*numPlanes)+(numPlanes*numLast)+i];

        // Step 3.4.3: Store each point to the vector.
        fieldPoints.push_back(point);
      }
      
      // Step 3.3.4: Update the total number of points already read.
      numLast += numPt;

      // Step 3.5: Assign the data to maps, and then map to pF.
      // Clear the vector fieldPoints for next iteration.  
      fieldMap[psiNormal[j]] = fieldPoints;
      pF.fieldPointsOnFlux = fieldMap;
      fieldPoints.clear();
    }
    fieldData.push_back(pF);
  }
  
  // Step 4: Return the field data.
  return fieldData;
}
