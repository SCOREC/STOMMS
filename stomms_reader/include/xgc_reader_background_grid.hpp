#ifndef XGC_READER_BACKGROUND_GRID
#define XGC_READER_BACKGROUND_GRID

#include "xgc_reader_utility.hpp"
#include "Omega_h_adios2.hpp"

class EqdskGridData{
  public:
    EqdskGridData(){};
    EqdskGridData(const adios2::IO& io, const adios2::Engine& reader, std::string name);

    /**
     * Function to return a vector of R coordinates of the grid.
     */ 
    const std::vector <double>& getGridPointsR() const;

    /**
     * Function to return a vector of Z coordinates of the grid.
     */ 
    const std::vector <double>& getGridPointsZ() const;
   
    /**
     * Function to return a vector of psi field on grid points.
     * Size of psi vector = size of R vector* size of Z vector.
     */ 
    const std::vector <double>& getPsiFieldOnGrid() const;

    /**
     * Function to return a vector of psi flux from Eqdsk.
     */
    const std::vector <double>& getPsiArray()const;

    /**
     * Function to return a vector of poloidal current from Eqdsk.
     */
    const std::vector <double>& getPoloidalCurrentArray() const;
 
    /**
     * Function to return a vector of R coordinates of the limiter.
     */
    const std::vector <double>& getLimiterR() const;

    /**
     * Function to return a vector of Z coordinates of the limiter.
     */
    const std::vector <double>& getLimiterZ() const;
     
    /**
     * Function to return eqdsk domain box.
     * An array of size 4.
     * Contains rMin, zMin, rMax, zMax.
     */
    std::array <double,4> getDomainBox() const; 

    /**
     * Function to return global PSPLINE psi spline coefficients.\n
     * Number of coefficients = 4*rGridPoints.size()*zGridPoints.size()
     */
    const std::vector <double>& getPsplinePsiCoefficients() const;
 
    /**
     * Function to return PSPLINE poloidal current spline coefficients.\n
     * Number of coefficients = 2*psi.size()
     */
    const std::vector <double>& getPsplineCurrentCoefficients() const;

    /**
     * Function to return a vector of all the bicubic spline coefficients in each grid cell.\n
     * It contains 16 coefficients for each cell. 
     */ 
    const std::vector <double>& getBicubicSplineCoefficients() const;

    /**
     * Function to return an array of bicubic spline coefficients for a grid cell.
     * @param rIndex: index of grid cell along R. Ranges from 0 to rGridPoints.size()-1
     * @param zIndex: index of grid cell along Z. Ranges from 0 to zGridPoints.size()-1
     */ 
    std::array <double, 16> getBicubicSplineCoefficientsInCell(int rIndex, int zIndex) const;

    /**
     * Function to return an array of psi values at Chebyshev points for a grid cell.
     * @param rIndex: index of grid cell along R. Ranges from 0 to rGridPoints.size()-1
     * @param zIndex: index of grid cell along Z. Ranges from 0 to zGridPoints.size()-1
     */ 
    std::array <double, 16> getPsiAtChebyshevPointsInCell(int rIndex, int zIndex) const;

  private:
    // Set Internal Data
    /**
     * Function to verify that all required variables exist in the adios2 file.
     */ 
    void verifyGridData();

    /**
     * Function to set psi field on a grid from the adions2 file.\n
     * Reads and sets the grid data (coordinates) and psi field on those points.
     */ 
    void setPsiFieldOnGrid();

    /**
     * Function to set 1D arrays data to eqdsk data.\n
     * Current Support: Psi array and Poloidal Current array.
     */
    void setDataArrays(); 

    /**
     * Function to set domain box limits. Its an array of size 4.\n
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
     * number of coefficients = 4*(# of r grid points)*(# of z grid points)
     */ 
    void setPsiSplineCoefficients();

    /**
     * Function to set spline coefficients for poloidal current. The 
     * coefficients are generated from PSPLINE ezspline routine.
     * number of coefficients = 2*(size of 1D poloidal current array)
     */ 
    void setPoloidalCurrentSplineCoefficients();

    /**
     * Function to set bicubic spline coefficients for psi. The coefficients 
     * are generated for each grid cell in the domain.
     * 16 coefficients for every box.
     * Number of coefficients = 16 * (# of r grid points - 1)*(# of z grid points - 1)
     */ 
    void setBicubicSplineCoefficients();

    /**
     * Function to set psi values at Chebyshev points of each grid cell.
     * number of points = 16 * (# of r grid points - 1)*(# of z grid points - 1).
     * This might be needed if bicubic spline coefficients are evaluated
     * in XGC instead of being provided in adios2 file. Will review it
     * future to see what data to keep and what to discard. (2026-07-21).
     */  
    void setPsiAtChebyshevPoints();

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
    std::vector <double> psiAtChebyshevPoints;

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
    std::vector <double> bicubicSplineCoefficients;
};

#endif
