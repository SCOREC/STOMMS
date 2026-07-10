#ifndef XGC_READER_BACKGROUND_GRID
#define XGC_READER_BACKGROUND_GRID

#include "xgc_reader_utility.hpp"
#include "Omega_h_adios2.hpp"

class EqdskGridData{
  public:
    EqdskGridData(){};
    EqdskGridData(const adios2::IO& io, const adios2::Engine& reader, std::string name);
  private:
    // Set Internal Data
    /**
     * Function to verify that all required variables exist in the adios2 file.
     */ 
    void verifyGridData();

    /**
     * Function to set psi field on a grid from the adions2 file.
     * Reads and sets the grid data (coordinates) and psi field on those points.
     */ 
    void setPsiFieldOnGrid();

    /**
     * Function to set 1D arrays data to eqdsk data.
     * Current Support: Psi array and Poloidal Current array.
     */
    void setDataArrays(); 

    /**
     * Function to set domain box limits. Its an array of size 4.
     * Contains rMin, zMin, rMax, zMax.
     */ 
    void setDomainBox();

    /**
     * Function to set coordinates of limiter (wall curve).
     */ 
    void setLimiter();

    /**
     * Function to coordinates of plasma boundary from eqdsk file.
     */ 
    void setPlasmaBoundary();

    /**
     * Function to set coordinates of different boundaries from the eqdsk file.
     * This includes domain box, limiter(wall curve) points, and plasma boundary
     * points. Plasma boundary is separatrix curve read directly from eqdsk data
     * and its not the separatrix curve from our curve generation routines.
     */ 
    void setBoundaryData();

    /**
     * Function to set spline coefficients for psi grid. The coefficients
     * are generated from PSPLINE ezspline routine.
     * # of coefficients = 4*(# of r grid points)*(# of z grid points)
     */ 
    void setPsiSplineCoefficients();

    /**
     * Function to set spline coefficients for poloidal current. The 
     * coefficients are generated from PSPLINE ezspline routine.
     * # of coefficients = 2*(size of 1D poloidal current array)
     */ 
    void setPoloidalCurrentSplineCoefficients();

    /**
     * Function to set PSPLINE spline coefficients for eqdsk data.
     * Sets psi spline and poloidal current spline coefficients.
     */ 
    void setSplineCoefficients();

    // Adios2 file info
    adios2::IO ioGrid;
    adios2::Engine readerGrid;
    std::string gridName;

    // Psi Grid Data
    std::vector <double> rGridPoints;
    std::vector <double> zGridPoints;
    std::vector <double> psiGrid;

    // Eqdsk Psi and Poloidal Current Arrays
    std::vector <double> psi;
    std::vector <double> poloidalCurrent;

    // Physical coordinates (limiter and plasma boundary points)
    // Limiter is basically wall curve given in eqdsk file.
    // plasma boundary (bdry) is separatrix boundary given in
    // eqdsk (not the one we traced from our calculations)
    std::vector <double> rLimiterPoints;
    std::vector <double> zLimiterPoints;
    std::vector <double> rBdryPoints;
    std::vector <double> zBdryPoints;
    std::vector <double> domainBox;

    // Splines Coefficients
    std::vector <double> psiSplineCoefficients;
    std::vector <double> currentSplineCoefficients;
};

#endif
