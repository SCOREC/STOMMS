#ifndef MAGNETICGEOMTRYSTELLARATOR_H
#define MAGNETICGEOMTRYSTELLARATOR_H

#include "ncFile.h"
#include "ncVar.h"
#include "magneticGeometry.h"

using namespace netCDF;

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
    double getPsiCoreBoundary() const override;

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

#endif
