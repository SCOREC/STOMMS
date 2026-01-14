#ifndef MAGNETICGEOMETRYTOKAMAK_H
#define MAGNETICGEOMETRYTOKAMAK_H

#include "magneticGeometry.h"
#include "gfileUtil.h"

/*
 * A class to hold magnetic geometry of Tokamak.
 */
class MagneticGeometryForTokamak: public MagneticGeometry{
  public:
    MagneticGeometryForTokamak(const WallCurve& wall, const bool& useReversePsi);

    /*
     * A function to return psi value of the axis in the tokamak domain.
     */
    double getPsiAxis() const override;

    double getPsiCoreBoundary() const override;

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

    // DO COMMENTING LATER
    const Model& getModel() const override;
    const std::vector <Plane>& getPlanes() const override;

   
  private:
    WallCurve wallCurve;
    bool reversePsi;
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints

    // For temporary place holder
    Model model;
    std::vector <Plane> planes; 
};

#endif
