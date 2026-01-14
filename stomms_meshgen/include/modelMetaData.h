#ifndef MODELMETADATA_H
#define MODELMETADATA_H

#include "input.h"
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
    PlaneMetaData(const FluxData& f, double angle);

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

  private:
    std::vector <double> fluxValues;  // vector of psi values of desired flux curves on the plane. 
    double toroidalAngle;  // Toroidal angle of the plane.
    std::vector <int> fluxMeshSize;  // Desired number of mesh points on each flux curve. fluxMeshSize.size() == fluxValues.size(). 
};

// class ModelMetaData holds the metadata information for all the poloidal planes.
class ModelMetaData{
  public:
    ModelMetaData(){};
    ModelMetaData(const Inputs& input);
    /*
     *  Function to return a vector containing all the planes with their meta data.
     */ 
    const std::vector <PlaneMetaData>& getPlanesContainer() const;
  
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
