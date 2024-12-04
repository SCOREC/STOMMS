#ifndef INPUT_H
#define INPUT_H

#include <netcdf>
#include "ncFile.h"
#include "ncVar.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <assert.h>
#include <math.h>

using namespace netCDF;

// Struct VmecData contains all the input VmecData.
struct VmecData{
  double majorR;  // Major radius of the reactor.
  double minorR;  // Minor radius of the reactor.
  int nSurf;  // Number of poloidal flux surfaces.
  int nMode;  // Number of modes for Fourier series.
  std::vector <double> R;  // Vector of cosines coeffiecents of R for Fourier series.
  std::vector <double> Z;  // Vector of sines coeffiecents of Z for Fourier series.
  std::vector <double> L;  // Vector of sines of lambdas coeffiecents for Fourier series.
  std::vector <double> psi;  // Vector of list of psi values of flux surfaces.
  std::vector <double> xm;   // poloidal modes.
  std::vector <double> xn;   // Toroidal modes.
};

// Struct bmwData contains the data from BMW file.
struct BmwData{
  // Populate it as we move forward.
};

// Struct eqdskData contains the magnetic field information from eqdsk file.
struct EqdskData{
  // Fill it in as we move forward.
};

// A struct to hold the input flux curves data. 
struct FluxData{
  std::vector <double> fluxInput;  // A vector to hold the input normalized psi values of flux curves.
  std::map <double, int> fluxMeshSize;  // A map between the flux normalized value and number of points one each flux curve.
}; 

// A struct to hold the input plane angles (converted to radians).
struct PlaneData{
  std::vector <double> planeInput;  // A vector to hold the input plane angles (converted in radians).
};

// A struct to contain all the input data from different set of files.
struct InputData{
  VmecData vm;  // Read the VMEC data for construction of model inside last closed flux curve.
  FluxData fd;  // Read the flux data from input files (fluxFile, meshSizeFile).
  PlaneData pd;  // Read the plane data from the input file (planeFile).
};

// Class args handles all the input information. This includes:
// magnetic field information, modeling and mesh parameters,
// any kind of control parameters.
class args{
  public:
    args(int argc, char* argv[]);

    // Input parameters
    std::string inputFile;
    std::string vmecFile;	// VMEC file to load magnetic field for stellarator core region 
    std::string fluxFile;	// Input file containing the number of flux curves and their flux indices 
    std::string planeFile;	// Input file containing the number and toroidal position (degrees) of the planes
    std::string meshSizeFile;	// Input file to define the mesh size on each flux curve in terms of number of desired points on flux curves

    // Variables and containers for internal use
    double psiAxis, psiLCF;	// psi values of Opoint and last closed flux curve.
    bool vmecFileFound;
    /*
     * Hierarchy in Input data to understand how to call data from other parts of code.
     * a (all input data including parameters under this umbrella)
     * .. in (input data from different input files to setup physics)
     * ..... vm  (vmec data)
     * ........ vmec data attributes (see struct VmecData)
     * ..... fd  (flux curves data)
     * ........ flux data attributes (see struct FluxData)
     * ..... pd  (plane data)
     * ........ plane data attributes (see struct PlaneData)
    */ 
    InputData in;  // Read the input data from different input files in this one.

    // Public member functions
    void setPsiBounds();
  private:
    /*
     * A function to set the default values of the parameters.
    */  
    void setDefaultValues();

    /*
     * Read the values from the inpput file and set it to parameters.
    */ 
    void setValuesFromInputFile(); 

    /*
     * Read different input files (flux, planes, mesh sizes) and set them to containers for further use.
    */  
    void setValuesForLocalUse();

    /*
     * Read input VMEC file and store relevant data in struct vmecData.
     * returns the struct vmecData vm.
    */ 
    VmecData readVmecFile();
    
    /*
     * Read the flux input file and check its validity.
     * returns a vector containing the normalized psi values of desired flux curves.
    */
    std::vector<double> readFluxFile();

    /*
     * Read the planes input files and check its validity.
     * converts the given angles in degrees to radians.
     * returns a vector containing the toroidal angles of the desired poloidal planes.
    */  
    std::vector<double> readPlaneFile();

    /*
     * Read the input for desired number of mesh vertices on each flux curve.
     * set them to a map between flux normalized psi value and desired number of mesh vertices on that flux.
     * returns a map between flux normalized psi value and desired number of mesh vertices on that flux.
    */
    std::map <double, int> readMeshSizeOnFlux();    
};

#endif
