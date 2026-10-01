#ifndef BMWDATA_H
#define BMWDATA_H

#include "ncFile.h"
#include "ncVar.h"
#include "ncDim.h"

using namespace netCDF;

class BmwData{
  public:
    BmwData(){};
    BmwData(const std::string& bmwFileName);
  private:
    // Grid Data
    int numR;
    int numZ;
    int numPhi;
    double rMin;
    double rMax;
    double zMin;
    double zMax;
   
    // Variables to set grid data
    std::vector <double> rPoints;
    std::vector <double> zPoints;
    std::vector <double> phiPoints;

    // Field Data
    int nFieldPeriods;
    std::vector <double> bR;
    std::vector <double> bPhi;
    std::vector <double> bZ;

    // Functions to read in data
    void setGridData(const NcFile& bmwFile);
    void setMagneticFieldData(const NcFile& bmwFile);

    // Set grid points
    void setGridPoints();
};

#endif
