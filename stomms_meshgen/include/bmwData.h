#ifndef BMWDATA_H
#define BMWDATA_H

#include "ncFile.h"
#include "ncVar.h"
#include "ncDim.h"
#include <array>
#include <cmath>

using namespace netCDF;

extern "C"{
  void  init_b_spline_(const double* rgrid, const double* zgrid, const double* phigrid,
        int* nr, int* nz, int* nphi, const double* br, const double* bz, const double* bphi);

  void eval_b_field_(const double* r, const double* z, const double* phi, double* B, int* ier);
}

class BmwData{
  public:
    BmwData(){};
    BmwData(const std::string& bmwFileName);
    const int& getNumPointsR() const;
    const int& getNumPointsZ() const;
    const int& getNumPointsPhi() const;
    std::array <double,4> getDomainBox() const;
    const std::vector <double>& getPointsR() const; 
    const std::vector <double>& getPointsZ() const;
    const std::vector <double>& getPointsPhi() const;
    const std::vector <double>& getFieldBr() const;
    const std::vector <double>& getFieldBz() const;
    const std::vector <double>& getFieldBphi() const;
    std::array <double, 3> getMagneticFieldAtPoint(double r, double z, double phi) const;
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
    std::vector <double> phiPointsRadian;

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
