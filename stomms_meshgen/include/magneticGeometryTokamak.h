#ifndef MAGNETICGEOMETRYTOKAMAK_H
#define MAGNETICGEOMETRYTOKAMAK_H

#include "magneticGeometry.h"
#include "gfileUtil.h"
#include "genFluxCurvesEqdsk.h"
#include "modelDataEqdsk.h"

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
     * @param input: class holding all the input data.
     */ 
    MagneticGeometryForTokamak(const ModelMetaData& modelMetaData, const WallCurve& wall, const Inputs& input);

    /**
     * A function to return psi value of the axis in the tokamak domain.
     * @return psi value of the axis.
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

    /**
     * Function to return magnetic field data on the background grid.
     */
    const GridFieldData& getGridFieldData() const override;    

  private:
    // Input data.
    WallCurve wallCurve;  // physical wall curve.
    bool reversePsi;  // mutliplies psi field with -1 if true.
    std::vector <double> psiNormList;

    // Derived Data.
    std::map<int , std::vector<PhysicsPoint>> oPoints;  // map between plane number and OPoints
    std::map<int , std::vector<PhysicsPoint>> xPoints;  // map between plane number and XPoints
    double psiAxis;
    double psiCoreBoundary;
    std::vector <double> psiValuesClosed;
    std::vector <double> psiValuesSeparatrix;
    std::vector <double> psiValuesOpen;
    std::vector <Flux> closedCurves;
    std::vector <Flux> separatrixCurves;
    PlaneMetaData planeMetaData;
    GridFieldData gridData;  // background grid data.
    ModelEqdsk modelEqdsk;
    
    // Internal functions:
    void classifyPsiValues(const Inputs& in);
    void genFluxCurves(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, const WallCurve& wall); 
};

#endif
