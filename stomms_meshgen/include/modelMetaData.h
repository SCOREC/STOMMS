#ifndef MODELMETADATA_H
#define MODELMETADATA_H

#include "input.h"
#include "modelTopology.h"

/**
 * The class PlaneMetaData contains the informtion on the magnetic geometry (mg), desired
 * flux curves, and desired planes.
 */
class PlaneMetaData{
  public:
    PlaneMetaData(){};
    /**
     * Constructor gets the flux data (f)plane toroidal angle to setup metadata on the plane.
     * @param f: the struct FluxData that constains the list of desired flux curves amd
     *           corresponding mesh sizes (number of desired vertices) on the flux curves.
     * @param angle: toroidal angle of the poloidal plane.
     */ 
    PlaneMetaData(const FluxData& f, double angle);

    /**
     * Function to return the psi values of the desired flux curves on the poloidal plane.
     */ 
    const std::vector<double>& getPlaneFluxValues();

    /**
     * Function to return the toroidal angle of the poloidal plane.
     */ 
    const double& getPlaneToroidalAngle();

    /**
     * Function to return a vector of desired node spacing on each flux curve
     * on the poloidal plane.
     */ 
    const std::vector <double>& getPlaneFluxSizes();

    /**
     * Function to get the mesh size spacing on a flux given the psi normalized value of the flux.
     * @param psiNorm: Input psi normalized value.
     * @return mesh spacing on the flux curve with psi normalized value.
     */ 
    double getNodeSpacingAtFlux(double psiNorm);
  private:
    std::vector <double> fluxValues;  // vector of psi values of desired flux curves on the plane. 
    double toroidalAngle;  // Toroidal angle of the plane.
    std::vector <double> fluxMeshSize;  // Desired node spacing on each flux curve. fluxMeshSize.size() == fluxValues.size(). 
};

/** 
 * class ModelMetaData holds the metadata information for all the poloidal planes.
 */
class ModelMetaData{
  public:
    /**
     * Constructor to create an object of ModelMetaData.
     */ 
    ModelMetaData(){};

    /**
     * Constructor to create an object of ModelMetaData using input information.
     * @param input: class holding all the input data.
     */ 
    ModelMetaData(const Inputs& input);

    /**
     *  Function to return a vector containing all the planes with their meta data.
     */ 
    const std::vector <PlaneMetaData>& getPlanesContainer() const;
  
    /**
     *  Function to return the metadata on a individual plane by index.
     *  @param index: Takes the index value of the plane (0 to nPlanes-1) as input.
     *  @return an object of PlaneMetaData.
     */ 
    const PlaneMetaData& getPlaneMetaDataByIndex(int index) const;

    /**
     *  Function to get toroidal angles of all the planes. Returns a vector of the angles in radians.
     */  
    const std::vector <double>& getToroidalAnglesMetaData();
  private:
    std::vector <PlaneMetaData> planesContainer;  // vector to hold all planes meta data.
    std::vector <double> planesToroidalAngles;  // vector to hold toroidal angles of the planes.
};

#endif
