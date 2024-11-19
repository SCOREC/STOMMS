#ifndef STOMMS_H
#define STOMMS_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelTopology.h"

class PlaneGeometry{
  public:
    PlaneGeometry(const FluxData& f, const MagneticGeometry& magGeom, double angle);
    const std::vector<double>& getPlanefluxValues();
    const double& getPlaneToroidalAngle();
    const std::vector <int>& getPlaneFluxSizes();
    const std::vector <PhysicsPoint>& getPlaneOpoints();
    const std::vector <PhysicsPoint>& getPlaneXpoints();
  private:
    std::vector <double> fluxValues;
    double toroidalAngle;  // Toroidal Angle of the plane.
    std::vector <int> fluxMeshSize;  // Desired number of mesh points on each flux curve. fluxMeshSize.size() == fluxValues.size(). 
 
    // PhysicsPoint contains both the physical coordinates and psi value of the point.
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;
};

class STOMMS{
  public:
    STOMMS(const MagneticGeometry& magGeom);
    ~STOMMS();
    void addPlane(PlaneGeometry pg);
    const std::vector <PlaneGeometry>& getPlanesContainer();

  private:
    MagneticGeometry mg;
    std::vector <PlaneGeometry> planes;
    pProgress prog;  // Simmetrix handler to show execution details.
    //Model model;
    //generateModel();
};
#endif

