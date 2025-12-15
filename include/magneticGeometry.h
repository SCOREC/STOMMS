#ifndef MAGNETICGEOMETRY_H
#define MAGNETICGEOMETRY_H

#include <memory>
#include "input.h"
#include "criticalPointsEqdsk.h"
#include "physicalGeometry.h"
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
 * Abstract Base class for MagneticGeometry. 
 * The derived class will depend on the reactor type.
 * Right now, we plan to have two derived classes.
 * One for the tokamak, and for the stellarator.
 */
class MagneticGeometry{
  public:
    virtual ~MagneticGeometry() = default;
    
    /*
     * A function to return psi value of the axis in the domain.
     */ 
    virtual double getPsiAxis() const = 0;

    /*
     * A function to return psi value of the last closed flux curve in the domain.
     */ 
    virtual double getPsiLCFS() const = 0;

    /*
     * A function to return the reactor type.
     */ 
    virtual ReactorType getReactorType() const = 0;

    /*
     * Function to get a map between plane number and vector of OPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of OPoints on the plane.   
     */ 
    virtual const std::map<int, std::vector<PhysicsPoint>>& getOPoints() const = 0;

    /*
     * Function to get a map between plane number and vector of XPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of XPoints on the plane.   
     */ 
    virtual const std::map<int, std::vector<PhysicsPoint>>& getXPoints() const = 0;

    /*
     * Function to return the VMEC data. All required VMEC data is
     * in the VmecData class.
     */ 
    virtual const VmecData& getVmecData() const = 0;
};

/*
 * A class to hold magnetic geometry of Stellarators. 
 */
class MagneticGeometryForStellarator: public MagneticGeometry{
  public:
    /*
     * Constructor to set magnetic geometry information from stellarator to class MagneticGeometry.
     * const std::string& vmecFileName (in): vmec file name containing all the vmec data.
    */ 
    MagneticGeometryForStellarator(const std::string& vmecFileName);

    /*
     * A function to return psi value of the axis in the vmec domain.
    */ 
    double getPsiAxis() const override;

    /*
     * A function to return psi value of the last closed flux curve in the vmec domain.
    */ 
    double getPsiLCFS() const override;

    /*
     * Return the reactor type(Stellarator for this class).
    */
     ReactorType getReactorType() const override;

    /*
     * Function to get a map between plane number and vector of OPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of OPoints on the plane.   
    */ 
    const std::map<int, std::vector<PhysicsPoint>>& getOPoints() const override;

    /*
     * Function to get a map between plane number and vector of XPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of XPoints on the plane.   
     */ 
    const std::map<int, std::vector<PhysicsPoint>>& getXPoints() const override;

    /*
     * Function to return the VMEC data. All required VMEC data is
     * in the VmecData class.
    */ 
    const VmecData& getVmecData() const override;

  private:
    std::string vmecFile;    
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints
    VmecData vmec;  // VMEC data (Stellarator core region)
    
    /*
     * Read input VMEC file and store relevant data in struct vmecData.
     * returns the struct vmecData vm.
     */
    VmecData readVmecData();
}; 

/*
 * A class to hold magnetic geometry of Tokamak.
 */
class MagneticGeometryForTokamak: public MagneticGeometry{
  public:
    MagneticGeometryForTokamak(const WallCurve& wall, const bool& useReversePsi);

    /*
     * A function to return psi value of the axis in the tokamak domain.
     */ 
    double getPsiAxis() const override {return 0.0;};

    double getPsiLCFS() const override {return 0.0;};

    ReactorType getReactorType() const override {return ReactorType::Tokamak;};
    
    /*
     * Function to get a map between plane number and vector of OPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of OPoints on the plane.   
    */ 
    const std::map<int, std::vector<PhysicsPoint>>& getOPoints() const override;

    /*
     * Function to get a map between plane number and vector of XPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of XPoints on the plane.   
     */ 
    const std::map<int, std::vector<PhysicsPoint>>& getXPoints() const override;

    /*
     * Function to return the VMEC data. All required VMEC data is
     * in the VmecData class.
    */ 
    const VmecData& getVmecData() const override;
  private:
    WallCurve wallCurve;
    bool reversePsi; 
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints
};

/*
 * Function to set magnetic geometry. This function makes decision based on type
 * of reactor, and set magnetic geometry accordingly.
 * const Inputs& input (in): class holding all the input data
 */
std::unique_ptr <MagneticGeometry> setMagneticGeometry(const Inputs& input, const PhysicalGeometry& physicalGeometry);

#endif
