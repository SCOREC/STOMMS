#include <netcdf>
#include "ncFile.h"
#include "ncVar.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "model.h"

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
using namespace netCDF;

//From a given Vmec File, generate the model of the core region of stellarator
pGModel generateCoreSimModel(args* a)
{
  // Step 1: Read all the variables in vmec file
  NcFile vmecFile(a->vmecFile, NcFile::read); 

  // netCDF Variables. First four hold single values and the rest are arrays of data.
  NcVar vmecMinor, vmecMajor, vmecSurf, vmecMode, vmecR, vmecZ, vmecL, vmecXm, vmecXn, vmecPsi;    

  // Step 2: Read required variables from vmec file
  vmecMinor = vmecFile.getVar("Aminor_p");
  vmecMajor = vmecFile.getVar("Rmajor_p");
  vmecSurf = vmecFile.getVar("ns");
  vmecMode = vmecFile.getVar("mnmax");
  vmecR = vmecFile.getVar("rmnc");
  vmecZ = vmecFile.getVar("zmns");
  vmecL = vmecFile.getVar("lmns");
  vmecXm = vmecFile.getVar("xm");
  vmecXn = vmecFile.getVar("xn");
  vmecPsi = vmecFile.getVar("phi");


  // Step 3: Assign netCDF variables to local variables and arrays
  double avmajr, avminr;
  int i, nsurf, nmode;

  vmecMinor.getVar(&avminr);
  vmecMajor.getVar(&avmajr);
  vmecSurf.getVar(&nsurf);
  vmecMode.getVar(&nmode);

  std::vector <double> R, Z, L, xm, xn, psi;
  R.resize(nsurf*nmode);
  Z.resize(nsurf*nmode);
  L.resize(nsurf*nmode);
  xm.resize(nmode);
  xn.resize(nmode);
  psi.resize(nsurf);

  vmecR.getVar(R.data());
  vmecZ.getVar(Z.data());
  vmecL.getVar(L.data());
  vmecXm.getVar(xm.data());
  vmecXn.getVar(xn.data());
  vmecPsi.getVar(psi.data());

  // Step 4: Create an object to hold Vmec flux data
  pVmecFlux vf = VmecFlux_create(avmajr, avminr, nsurf, nmode, R.data(), Z.data(), L.data(), xm.data(), xn.data());

  // Step 5: Read number of fluxs (nrho) on each poloidal plane and number of poloidal planes (nzeta) 
  // along with their indices (rhos) and toroidal angle (zetas) from the fluxFile and planeFile.
  // Also, read psi values at O-point and last closed flux curve from VMEC file and use them to convert
  // normalized psi to actual psi. 
  double psiAxis = psi[0];
  double psiLCF = psi[nsurf-1];
  std::vector <int> rhos = readFluxFile(nsurf, a);
  std::vector <double> zetas = readPlaneFile(a);
  const int nrho = rhos.size(), nzeta = zetas.size();
  VmecFlux_setFluxIndices(vf, nrho, rhos.data());
  VmecFlux_setToroidalAngles(vf, nzeta, zetas.data());

  // Step 6: Create model entities from the vmec physics data.
  pGModel model = simModelFromVmec(vf, nrho, nzeta, rhos.data(), zetas.data());

  // Step 7: Write the model (.smd) on disk for visualization.
  GM_write(model, "vmec.smd", 0, 0);

  return model;
}

// From Vmec flux data (vf), flux indices (nrho and rhos), and, poloidal planes (nzeta and zetas)
// generate a Simmetrix model (pGModel model)
pGModel simModelFromVmec(pVmecFlux vf, int nrho, int nzeta, const int *rhos, const double *zetas)
{
  // Step 1: Declare and create Model from vf data
  pGModel model = GM_new(0);
  pGIPart gp = GM_createVmecPart(model, vf, 2); 

  // Step 2: Sanity check (Verifying some of the model entities)
  int rho, rho0, rho1;
  double zeta;
  pGVertex gv = VmecFlux_opointVertex(vf, zetas[2]);
  VmecFlux_opointVertexInfo(vf, gv, &zeta);
  assert(zeta == zetas[2]);
  pGEdge ge = VmecFlux_poloidalEdge(vf, rhos[2], zetas[2]);
  VmecFlux_poloidalEdgeInfo(vf, ge, &rho, &zeta);
  assert(rho == rhos[2] && zeta == zetas[2]);
  pGFace gf = VmecFlux_poloidalFace(vf, rhos[2], zetas[2]);
  VmecFlux_poloidalFaceInfo(vf, gf, &rho0, &rho1, &zeta);
  assert(rho0 == rhos[2] && rho1 == rhos[3] && zeta == zetas[2]);

  return model;
}

// Read flux file and check its validity
std::vector<int> readFluxFile(int nsurf, args* a)
{
  int numFlux;  // Number of flux curves from the first line of the file.
  int fluxIndex;  // To read the flux indices one by one from the file
  std::vector <int> fluxIndices;  // A vector storing the flux indices from the file.
  std::ifstream fluxInput(a->fluxFile);  // Load the file.

  // Step 1: If can't open the file, exit the program with an error message.
  if (!fluxInput.is_open())
  {
    std::cout << "Error opening the flux input file " << a->fluxFile << "\n";
    exit(1);
  }
  
  // Step 2: If the first line is empty, exit with an error message. Else, the value
  // from the first line is stored to numFlux
  if (!(fluxInput >> numFlux))
  {
    std::cout << "The given file for flux curves seems to be empty" << "\n";
    exit(1);
  }
  
  // Step 3: Read the values from the file and store them in fluxIndices vector.
  while(fluxInput >> fluxIndex)
    fluxIndices.push_back(fluxIndex);

  // Step 4: Make sure the given data is consistent.
  assert (numFlux == fluxIndices.size());

  // Step 5: Make sure there is no flux indices lesser than 0 or greater than the index of the last closed flux curve (nsurf-1).
  for (int i = 0; i < fluxIndices.size(); i++)
  {
    int index = fluxIndices[i];
    if (index < 0 || index >= nsurf)
    {
	fluxIndices.erase(fluxIndices.begin()+i);
        std::cout << " The flux index " << index << " is removed since it was either lesser than 0 or greater than the index(" << nsurf-1 <<") of last closed flux curve from VMEC file\n";
        i--;  // Makes sure to iterate over the element next to the deleted element.
    }
  }

  // Step 6: Returns the flux indices vector.
  return fluxIndices;
}

// Convert the normalized psi values to actual psi values. 
std::vector <double> convertNormToPsi(std::vector <double> normPsi, double psiAxis, double psiLCF)
{
  std::vector <double> psiValues;
  for (int i = 0; i < normPsi.size(); i++)
  {
    double psiNorm = normPsi[i];  // Normalized psi value
    double psi = psiNorm*(psiLCF - psiAxis) + psiAxis;
    psiValues.push_back(psi);
  }
  return psiValues;
}

// Read the planes file and check its validity.
// Converts the given angles in degrees to radians.
std::vector<double> readPlaneFile(args* a)
{
  int numPlanes;  // Number of poloidal planes from the first line of the file.
  double angle;  // To read the poloidal plane angles one by one from the file.
  std::vector <double> planeAngles;  // A vector storing the plane angles in degrees from the file.
  std::ifstream planeInput(a->planeFile);  //Load the file.

  // Step 1: If can't open the file, exit the program with an error message.
  if (!planeInput.is_open())
  {
    std::cout << "Error opening the plane input file " << a->planeFile << "\n";
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

// Setting model entities from simModel to respective planes
void getPlanes(pGModel model, std::vector <Plane>& planesContainer, args* a)
{
  // Step 1: Sort all the model faces according to plane number in a map
  // In map, the key is the plane # and the model data is stored in a vector 
  // containing the model faces.
  std::map <int, std::vector<pGFace>> planesMap = sortFacesbyPlanes(model, a);

  // Step 2: Iterate over the map and assign the data to each plane (using object Plane)
  std::map <int, std::vector<pGFace>>::iterator itr;
  for (itr = planesMap.begin(); itr != planesMap.end(); itr++)
  {
    Plane p;
    p.planeNumber = (itr->first)+1;
    p.modelFaces = itr->second;
    planesContainer.push_back(p);
  }
}

// From the simModel, sort the model entities by planes. This results defining
// each plane using its model entities (model faces for now).
std::map<int,std::vector<pGFace>> sortFacesbyPlanes(pGModel model, args* a) 
{
  // Step 1: Fetch the vmecFlux data (vf) from the model and also read the planes
  // information from the input plane file.
  pVmecFlux vf = GM_vmec(model);
  std::vector <double> zetas = readPlaneFile(a);

  // Step 2: Iterate over the model faces, read their toroidal angle and compare it
  // to data in planes vector (zetas) to sort the model faces according to their plane number.
  std::map <int, std::vector<pGFace>> planesMap;
  GFIter fIter = GM_faceIter(model);
  while (pGFace gFace = GFIter_next(fIter))
  {
    int rho0, rho1;
    double zeta;
    pGFace gf = gFace;

    // Setp 2.1: Read the toroidal angle of the model face.
    VmecFlux_poloidalFaceInfo(vf, gf, &rho0, &rho1, &zeta);
    int index = -1;

    // Step 2.2: Compare the angle with the angles provided in input file (read in vector zetas).
    // If similar found, push the model face to respective plane number in planesMap.
    for (int i = 0; i < zetas.size(); i++)
    {
      if (zetas[i] - zeta < 1e-16)
        index = i;
    }
    planesMap[index].push_back(gf);
  }
  GFIter_delete(fIter); 

  // Step 3: Returns the map between plane number and associated vector of model faces.
  return planesMap;
}
