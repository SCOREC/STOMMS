#ifndef MAGNETICGEOMTRYBMW_H
#define MAGNETICGEOMTRYBMW_H

#include "bmwData.h"
#include "magneticGeometry.h"

/**
 * A class to hold magnetic geometry of stellarators from BMW along 
 * with the construction of actual geometry (model). 
 */
class MagneticGeometryBmw: public MagneticGeometry{
  public:
    /**
     * Constructor to set magnetic geometry information from VMEC to class MagneticGeometry.
     * @param md: metadata needed for geometry construction.
     * @param bmwFileName: BMW file name containing all the vmec data.
     */ 
    MagneticGeometryBmw(const ModelMetaData& md, const std::string& bmwFileName);

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

    /**
     * Function to return magnetic field data on the background grid.
     */
    const GridFieldData& getGridFieldData() const override;

  private:
    std::string bmwFile;  // bmw file name    
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints
    BmwData bmw;  // BMW data

    // TEMPORARY. LATER REPLACE THEM WITH ACTUAL DATA
    //ModelVmec modelVmec;  // model data associated with vmec geometry  
    GridFieldData gridData;  // background grid data  
    Model dummyModel;
    std::vector <Plane> dummyPlanes; 
}; 

#endif

