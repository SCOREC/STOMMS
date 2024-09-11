#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <assert.h>
#include <math.h>

class args{
  public:
    args(int argc, char* argv[]);

    // Input parameters
    std::string inputFile;
    std::string vmecFile;	// VMEC file to load magnetic field for stellarator core region 
    std::string fluxFile;	// Input file containing the number of flux curves and their flux indices 
    std::string planeFile;	// Input file containing the number and toroidal position (degrees) of the planes
    std::string meshSizeFile;	// Input file to define the mesh size on each flux curve in terms of number of desired points on flux curves
    double meshSize;	        // Uniform mesh size defintion. For now (first iteration of code), will need to change it in future with better thing

    // Variables and containers for internal use
    double psiAxis, psiLCF;	// psi values of Opoint and last closed flux curve.
    std::vector <double> fluxInput;	// A vector to hold the input normalized psi values of flux curves.
    std::vector <double> planeInput;	// A vector to hold the input plane angles (converted in radians). 
    std::map <double, int> fluxMeshSize;	// A map between the flux normalized value and number of points one each flux curve.

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
