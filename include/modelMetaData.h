#ifndef MODELMETADATA_H
#define MODELMETADATA_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelTopology.h"

// The class PlaneMetaData contains the informtion on the magnetic geometry (mg), desired
// flux curves, and desired planes.
class PlaneMetaData{
  public:
    /*
     * Constructor gets the flux data (f), magnetic geometry (mg), and plane toroidal angle to setup
     * meta data on the plane.
     * const FluxData& f (in): The struct FluxData that constains the list of desired flux curves amd
     * 			       corresponding mesh sizes (number of desired vertices) on the flux curves.
     * const MagneticGeometry& magGeom (in): The given magnetic geometry (vmec, eqdsk, bmw etc.).
     * double angle: Toroidal angle of the poloidal plane.
     */ 
    PlaneMetaData(const FluxData& f, const MagneticGeometry& magGeom, double angle);

    /*
     * Function to return the psi values of the desired flux curves on the poloidal plane.
     */ 
    const std::vector<double>& getPlaneFluxValues();

    /*
     * Function to return the toroidal angle of the poloidal plane.
     */ 
    const double& getPlaneToroidalAngle();

    /*
     * Function to return a vector of desired number of vertices on each flux curve
     * on the poloidal plane.
     */ 
    const std::vector <int>& getPlaneFluxSizes();
 
    /*
     * Function to return the vector of O-points on the poloidal plane (future task).
     */ 
    const std::vector <PhysicsPoint>& getPlaneOpoints();

    /*
     * Function to return the vector of X-points on the poloidal plane (future task).
     */ 
    const std::vector <PhysicsPoint>& getPlaneXpoints();

    /*
     * Function to return the magnetic geometry.
     */ 
    const MagneticGeometry& getMagneticGeometry();
  private:
    MagneticGeometry mg;  // Magnetic geometry information.
    std::vector <double> fluxValues;  // vector of psi values of desired flux curves on the plane. 
    double toroidalAngle;  // Toroidal angle of the plane.
    std::vector <int> fluxMeshSize;  // Desired number of mesh points on each flux curve. fluxMeshSize.size() == fluxValues.size(). 
 
    // PhysicsPoint contains both the physical coordinates and psi value of the point.
    std::vector <PhysicsPoint> oPoints;
    std::vector <PhysicsPoint> xPoints;
};

// class ModelMetaData holds the metadata information for all the poloidal planes.
class ModelMetaData{
  public:
    /*
     * Function to add individual plane meta data to ModelMetaData.
     * PlaneMetaData pg (in): Takes the PlaneMetaData as input.
     */
    void addPlane(PlaneMetaData pg);

    /*
     *  Function to return a vector containing all the planes with their meta data.
     */ 
    const std::vector <PlaneMetaData>& getPlanesContainer();
  
    /*
     *  Function to return the metadata on a individual plane by index.
     *  int index (in): Takes the index value of the plane (0 to nPlanes-1) as input.
     */ 
    const PlaneMetaData& getPlaneMetaDataByIndex(int index);

    /*
     *  Function to get toroidal angles of all the planes. Returns a vector of the angles in radians.
     */  
    const std::vector <double>& getToroidalAnglesMetaData();
  private:
    std::vector <PlaneMetaData> planesContainer;  // vector to hold all planes meta data.
    std::vector <double> planesToroidalAngles;  // vector to hold toroidal angles of the planes.
};

#endif
