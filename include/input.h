#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <string>
#include <fstream>


class args{
  public:
    args(int argc, char* argv[]);

    // Input Parameters
    std::string inputFile;
    std::string vmecFile;	// VMEC file to load magnetic field for stellarator core region 
    std::string fluxFile;	// Input file containing the number of flux curves and their flux indices 
    std::string planeFile;	// Input file containing the number and toroidal position (degrees) of the planes
    double meshSize;	        // Uniform mesh size defintion. For now (first iteration of code), will need to change it in future with better thing

  private:
    void setValuesFromInputFile(); 
    void setDefaultValues();
    
};

#endif
