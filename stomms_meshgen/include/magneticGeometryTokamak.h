#ifndef MAGNETICGEOMETRYTOKAMAK_H
#define MAGNETICGEOMETRYTOKAMAK_H

#include "magneticGeometry.h"
#include "gfileUtil.h"

/**
 * A class to hold magnetic geometry of tokamaks along with the
 * construction of actual geometry (model).
 */
class MagneticGeometryForTokamak: public MagneticGeometry{
  public:
    /**
     * Constructor to set magnetic geometry information from tokamak to class MagneticGeometry.
     * @param wall: phsyical wall curve of the reactor. Needed for critical point search and 
     *              flux curves (open and separatrix) generation.
     * @param useReversePsi: parameter to determine if psi in the given eqdsk needs to be 
     *                       multiplied with -1 or not. Ensures minium at o-point.
     */ 
    MagneticGeometryForTokamak(const WallCurve& wall, const bool& useReversePsi);

    /**
     * A function to return psi value of the axis in the tokamak domain.
     */
    double getPsiAxis() const override;

    /**
     * A function to return psi value at the boundary of core region.
     */ 
    double getPsiCoreBoundary() const override;

    /**
     * A fucntion to get the reactor type of geometry (stellarator/tokamak).
     */ 
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

    /**
     * Function to get model associated with tokamak geometry.
     */ 
    const Model& getModel() const override;

    /**
     * Function to get all the geometric information on individual planes.
     * Only one plane for tokamaks.
     */ 
    const std::vector <Plane>& getPlanes() const override;

   
  private:
    WallCurve wallCurve;  // physical wall curve.
    bool reversePsi;  // mutliplies psi field with -1 if true.
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints

    // For temporary place holder
    Model model;
    std::vector <Plane> planes; 
};

#endif
