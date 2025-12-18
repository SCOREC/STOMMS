#ifndef XGC_READER_PLANE_H
#define XGC_READER_PLANE_H

#include "xgc_reader_topology.hpp"
#include "xgc_reader_classification.hpp"
#include "xgc_reader_reverse_classification.hpp"
#include "xgc_reader_nonaligned_vertices.hpp"
#include "xgc_reader_utility.hpp"

class PlaneMesh{
  public:
    /*
     * PlaneMesh Constructor.
     * Omega_h::Mesh& planeOmegahMesh (in): Omegah mesh on the plane.
     * adios2::IO& io (in): adios2 io to interact with adios2 input file.
     * adios2::Engine& reader (in): adios2 reader engine to read data from adios file.
     * std::string& name (in): Mesh name from adios2 file.
     * int planeNumber (in): plane number.
     */ 
    PlaneMesh(const Omega_h::Mesh& planeOmegahMesh, const adios2::IO& io, const adios2::Engine& reader, std::string name, int planeNumber);

    /*
     * Function to return Omega_h mesh on the plane.
     */ 
    const Omega_h::Mesh& getOmegahMesh() const;

    /*
     * Function to return plane number.
     */ 
    int getPlaneNumber();

    /*
     * Function to return curves at specific physics region. Only works with current 4 
     * types of regions in XGC (core = 0, SOL = 1, Priavte 1 = 2, Private 2 = 3). Once
     * we have a general definition of regions, move this function to class Model.
     * int physicsRegion (in): Physics region on which model curves are needed.
     */ 
    std::vector <ModelCurve> getCurvesAtPhysicsRegion(int physicsRegion) const;

    /*
     * Function to return an object of class ModelTopology set on the plane.
     */ 
    const ModelTopology& getModelTopology() const;

    /*
     * Function to return an object of class Model set on the plane.
     */ 
    
    const Model& getModel() const;

    /*
     * Function to return an object of class NonAlignedVertices set on the plane.
     */ 
    const NonAlignedVertices& getNonAlignedVertices() const;

    /*
     * Function to return an object of class MeshClassification set on the plane.
     */ 
    const MeshClassification& getMeshClassification() const;

    /*
     * Function to return an object of class ReverseClassification set on the plane.
     */ 
    const ReverseClassification& getReverseClassification() const;
  private:
    // Functions

    /*
     * Function to set mesh name to the plane.
     */ 
    void setMeshName(const std::string& name);

    // Variables
    adios2::IO ioPlane;
    adios2::Engine readerPlane;
    Omega_h::Mesh omegahMesh;
    int planeNum;
    std::string meshName;
    ModelTopology modelTopology;
    Model model;
    MeshClassification meshClassification;
    ReverseClassification reverseClassification;
    NonAlignedVertices nonAlignedVertices;

    // Variables & Function that might be helpful for current XGC version 
    // but we might need to redesign them in future
    // core = 0, SOL = 1, Priavte 1 = 2, Private 2 = 3,....
    std::map <int, std::vector <ModelCurve>> modelCurvesOnPhysicsRegions;

    /*
     * Function to set model curves on the above four physics regions.
     * Move it to class Model once general definition is decided.
     */ 
    void setModelCurvesOnPhysicsRegions();

    /*
     * Given a model curve, this function returns the physics region type it
     * is classified on.
     * ModelCurve modelCurve (in): Model curve for which region type is needed.
     */ 
    int getRegionTypeForCurve(ModelCurve modelCurve);
};

#endif
