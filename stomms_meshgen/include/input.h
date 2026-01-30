#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <assert.h>
#include <math.h>
#include "gfileUtil.h"

// A struct to hold the input flux curves data. 
struct FluxData{
  std::vector <double> fluxInput;  // A vector to hold the input normalized psi values of flux curves.
  std::map <double, double> fluxMeshSize;  // A map between the flux normalized value and vertex spacing on each flux curve.
}; 

// A struct to hold the input plane angles (converted to radians).
struct PlaneData{
  std::vector <double> planeInput;  // A vector to hold the input plane angles (converted in radians).
};

// A struct to contain all the input data from different set of files.
struct InputData{
  FluxData fd;  // Read the flux data from input files (fluxFile, meshSizeFile).
  PlaneData pd;  // Read the plane data from the input file (planeFile).
};

// Geometry Type - Based on input we decide it.
enum class ReactorType{
  Tokamak,
  Stellarator,
  None
};

// Class Inputs handles all the input information. This includes:
// magnetic field information, modeling and mesh parameters,
// any kind of control parameters.
class Inputs{
  public:
    Inputs();
    
    /*
     * Function to return limiter (wall curve) file name.
     */ 
    const std::string& getLimiterFile() const;

    /*
     * Function to VMEC file name.
     */ 
    const std::string& getVmecFile() const;

    /*
     * Function to return InputData struct which contains flux and planes info.
     */ 
    const InputData& getInputData() const;

    /*
     *  Function to return reactor type (Stellarator, Tokamak)
     */ 
    const ReactorType& getReactorType() const;

    /*
     * Function to check if psi is reverse or not.
     */
    const bool& useReversePsi() const; 
  private:
    // Input parameters
    std::string inputFile;
    std::string vmecFile;	// VMEC file to load magnetic field for stellarator core region.
    std::string eqdskFile;      // EQDSK file to load magnetic field for tokamaks. 
    std::string fluxFile;	// Input file containing the number of flux curves and their flux indices.
    std::string planeFile;	// Input file containing the number and toroidal position (degrees) of the planes.
    std::string meshSizeFile;	// Input file to define the mesh size on each flux curve in terms of number of desired points on flux curves.
    std::string limiterFile;    // Input file for the wall curve (limiter). Optional
    
    // Variables and containers for internal use
    bool reversePsi;            // To reverse the given psi values to get minimum psi at axis.
    double eqdPsiFactor;        // A factor to multipy the psi field.
    ReactorType reactorType;
 
   /*
     * Hierarchy in Input data to understand how to call data from other parts of code.
     * a (all input data including parameters under this umbrella)
     * .. in (input data from different input files to setup physics)
     * ..... fd  (flux curves data)
     * ........ flux data attributes (see struct FluxData)
     * ..... pd  (plane data)
     * ........ plane data attributes (see struct PlaneData)
    */ 
    InputData in;  // Read the input data from different input files in this one.

    // Member Functions
    
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
     * Read input tokamak equilibrium file (.eqd or geqdsk).
     */
    void initializeEqdskFile();

    /*
     * Read the input for desired mesh resolution on each flux curve.
     * set them to a map between flux normalized psi value and desired node spacing on that flux.
     * returns a map between flux normalized psi value and desired node spacing on that flux.
     */
    std::map <double, double> readMeshSizeOnFlux();
};

#endif
