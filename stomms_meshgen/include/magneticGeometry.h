#ifndef MAGNETICGEOMETRY_H
#define MAGNETICGEOMETRY_H

#include <memory>
#include "input.h"
#include "criticalPointsEqdsk.h"
#include "physicalGeometry.h"
#include "modelMetaData.h"
#include "magneticGeometryDataTypes.h"
#include "modelingVmec.h"

/*
 * Abstract Base class for MagneticGeometry. 
 * The derived class will depend on the reactor type.
 * Right now, we plan to have two derived classes.
 * One for the tokamak, and for the stellarator.
 */
class MagneticGeometry{
  public:
    virtual ~MagneticGeometry() = default;
    
    /**
     * A function to return psi value of the axis in the domain.
     */ 
    virtual double getPsiAxis() const = 0;

    /**
     * A function to return psi value of the xore region boundary in the domain.
     * Mainly needed for normalization. In VMEC, its last flux curve psi. In the 
     * case of tokamaks, its innermost separatrix curve (x-point).
     */ 
    virtual double getPsiCoreBoundary() const = 0;

    /**
     * A function to return the reactor type.
     */ 
    virtual ReactorType getReactorType() const = 0;

    /**
     * Function to get a map between plane number and vector of OPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of OPoints on the plane.   
     */ 
    virtual const std::map<int, std::vector<PhysicsPoint>>& getOPoints() const = 0;

    /**
     * Function to get a map between plane number and vector of XPoints.
     * Key in map: plane number (id)
     * Value in map: a vector of XPoints on the plane.   
     */ 
    virtual const std::map<int, std::vector<PhysicsPoint>>& getXPoints() const = 0;

    /**
     * Function to get model associated with geometry.
     */  
    virtual const Model& getModel() const = 0;

    /**
     * Function to get all the geometric information on individual planes.
     */ 
    virtual const std::vector <Plane>& getPlanes() const = 0;
};


/*
 * Function to set magnetic geometry. This function makes decision based on type
 * of reactor, and set magnetic geometry accordingly.
 * @param input: class holding all the input data.
 * @param physicalGeometry: information related to phsyical geometry. (right now just wall curve).
 * @param modelMetaData: metadata needed for geometry construction.
 * @return pointer to class MagneticGeometry.
 */
std::unique_ptr <MagneticGeometry> setMagneticGeometry(const Inputs& input, const PhysicalGeometry& physicalGeometry, const ModelMetaData& modelMetaData);

#endif
