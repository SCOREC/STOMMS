#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <string>
#include <fstream>


class args{
  public:

    // Member Functions
    args(int argc, char* argv[]);
    void set_values_from_input_file(); 

    // Input Parameters
    std::string inputFile;
    std::string vmecFile;	// VMEC file to load magnetic field for stellarator core region 

};

#endif
