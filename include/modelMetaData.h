#ifndef MODELMETADATA_H
#define MODELMETADATA_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelTopology.h"

class PlaneMetaData{
  public:
    PlaneMetaData(const FluxData& f, const MagneticGeometry& magGeom, double angle);
    const std::vector<double>& getPlaneFluxValues();
    const double& getPlaneToroidalAngle();
    const std::vector <int>& getPlaneFluxSizes();
    const std::vector <PhysicsPoint>& getPlaneOpoints();
    const std::vector <PhysicsPoint>& getPlaneXpoints();
    const MagneticGeometry& getMagneticGeometry();
  private:
    MagneticGeometry mg;
    std::vector <double> fluxValues;
    double toroidalAngle;  // Toroidal Angle of the plane.
    std::vector <int> fluxMeshSize;  // Desired number of mesh points on each flux curve. fluxMeshSize.size() == fluxValues.size(). 
 
    // PhysicsPoint contains both the physical coordinates and psi value of the point.
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;
};

#endif
