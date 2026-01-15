#ifndef MAGNETICGEOMTRYSTELLARATOR_H
#define MAGNETICGEOMTRYSTELLARATOR_H

#include "ncFile.h"
#include "ncVar.h"
#include "magneticGeometry.h"
#include "modelDataVmec.h"

using namespace netCDF;

/**
 * A class to hold magnetic geometry of stellarators along with the
 * construction of actual geometry (model). 
 */
class MagneticGeometryForStellarator: public MagneticGeometry{
  public:
    /**
     * Constructor to set magnetic geometry information from stellarator to class MagneticGeometry.
     * @param md: metadata needed for geometry construction.
     * @param vmecFileName: vmec file name containing all the vmec data.
     */ 
    MagneticGeometryForStellarator(const ModelMetaData& md, const std::string& vmecFileName);

    /**
     * A function to return psi value of the axis in the vmec domain.
     */ 
    double getPsiAxis() const override;

    /**
     * A function to return psi value of the last closed flux curve in the vmec domain.
     */ 
    double getPsiCoreBoundary() const override;

    /**
     * Return the reactor type(Stellarator for this class).
     */
     ReactorType getReactorType() const override;

    /**
     * Function to get a map between plane number and vector of OPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of OPoints on the plane.   
     */ 
    const std::map<int, std::vector<PhysicsPoint>>& getOPoints() const override;

    /**
     * Function to get a map between plane number and vector of XPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of XPoints on the plane.   
     */ 
    const std::map<int, std::vector<PhysicsPoint>>& getXPoints() const override;

    /**
     * Function to get model associated with stellarator geometry.
     */ 
    const Model& getModel() const override;

    /**
     * Function to get all the geometric information on individual planes.
     */ 
    const std::vector <Plane>& getPlanes() const override;
  private:
    std::string vmecFile;  // vmec file name    
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints
    VmecData vmec;  // VMEC data (Stellarator core region)
    ModelVmec modelVmec;  // model data associated with vmec geometry  
    
    /**
     * Read input VMEC file and store relevant data in struct vmecData.
     * returns the struct vmecData vm.
     */
    VmecData readVmecData();
}; 

#endif
