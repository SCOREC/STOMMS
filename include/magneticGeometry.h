#ifndef MAGNETICGEOMETRY_H
#define MAGNETICGEOMETRY_H

#include "input.h"
#include "ncFile.h"
#include "ncVar.h"
#include "gfileUtil.h"

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
  std::vector <double> iota;  // Vector of iota values corresponding to flux surfaces.
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
  // add data here as we move forward.
  double psiAxis;
  double psiSep;
};

/*
 * A class to hold all the magnetic geometries. Only VMEC is supported 
 * at the moemnt. Reads the data from args class that handles all the 
 * input data.
 */
class MagneticGeometry{
  public:
    /*
     * A fucntion to set magnetic geometry information (any kind of physics information given)
     * to class MagneticGeometry.
     * const args& a (in): class holding all the input data (magnetic geometry, modeling and meshing parameters etc.)
     */ 
    void setMagneticGeometry(const args& a);

    /*
     * A function to return vmec data (struct VmecData).
     */ 
    const VmecData& getVmecData();

    /*
     * A function to return psi value of the axis in the vmec domain.
     */ 
    const double& getPsiAxis();

    /*
     * A function to return psi value of the last closed flux curve in the vmec domain.
     */ 
    const double& getPsiLCFS();
  private:
    std::string vmecFileName;    

    VmecData vmec;  // Read the VMEC data for construction of model inside last closed flux curve.
    BmwData bmw;  // Future task
    EqdskData eqdsk;  // Read the EQDSK data for construction of tokamak models.
 

    /*
     * Read input VMEC file and store relevant data in struct vmecData.
     * returns the struct vmecData vm.
     */
    VmecData readVmecData();

    double psiAxis;  // psi value of the axis in the core region.
    double psiLCFS;  // psi value of the last closed flux curve in the core region.
}; 

#endif
