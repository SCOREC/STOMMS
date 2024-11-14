#ifndef STOMMS_H
#define STOMMS_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelTopology.h"

class PlaneGeometry{
  public:
    PlaneGeometry(const FluxData& f, const MagneticGeometry& magGeom, double angle);
    std::vector<double> getPlanefluxValues();
    double getPlaneToroidalAngle();
    std::map <double, int> getPlaneFluxSizes();
    std::vector <PhysicsPoint> getPlaneOpoints();
    std::vector <PhysicsPoint> getPlaneXpoints();
  private:
    std::vector <double> fluxValues;
    double toroidalAngle;  // Toroidal Angle of the plane.
    std::map <double, int> fluxMeshSize;  // Desired number of mesh points on each flux curve.   
 
    // PhysicsPoint contains both the physical coordinates and psi value of the point.
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;
};

class STOMMS{
  public:
    STOMMS(const MagneticGeometry& magGeom);
    void addPlane(PlaneGeometry pg);
    std::vector <PlaneGeometry> getPlanesContainer();
  private:
    MagneticGeometry mg;
    std::vector <PlaneGeometry> planes;
    //Model model;
    //generateModel();
};
#endif

