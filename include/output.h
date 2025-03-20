#ifndef OUTPUT_H
#define OUTPUT_H

#include "stommsMesh.h"
#include <adios2.h>
#include "Omega_h_build.hpp"
#include "Omega_h_mesh.hpp"
#include "Omega_h_meshsim.hpp"
#include "Omega_h_file.hpp"
#include "Omega_h_adios2.hpp"

class StommsOutput{
  public:
    StommsOutput(const StommsMesh& m);
  private:
    StommsMesh mesh;  // StommsMesh with underlying Simmetrix mesh and other meta data.
    pMesh simMesh;  // Simmetrix mesh
    pGModel simModel;  // Simmetrix model
    std::vector <PlaneMeshData> planes;  // Mesh data set on individual planes
    std::vector <Omega_h::Mesh> omegahMeshes;  // vector of omegah 2D planer meshes
    
    int meshDim = 2;

    // In future, read this from user's input
    int outputVtk = 0;
    int outputGmsh = 0;

    /*
     * Function to adjust mesh verter indices to make sure vertex indices 
     * start from 0 to nVertices-1.
     * std::vector <pVertex> v (in): Vector of mesh vertices on the poloidal plane.
     */  
    void setMeshIndices(std::vector <pVertex> v);  

    /*
     * Function to write Omegah planer meshes from Simmetrix mesh.
     */ 
    void writeOmegahMeshes();

    /*
     * Function to create 2D omegah mesh from mesh entity data on each plane.
     * const PlaneMeshData& plane (in): SImemtrix mesh data on plane.
     * returns Omegah mesh (Omega_h::Mesh).
     */ 
    Omega_h::Mesh simMesh2Omegah2D(const PlaneMeshData& plane);

    /*
     * Function to create 3D omegah mesh from 3D Simmetrix mesh.
     * returns Omegah mesh (Omega_h::Mesh).
     */ 
    Omega_h::Mesh simMesh2Omegah3D();

    /*
     * Function to write ADIOS2 file from omegah meshes and other input information.
     * ************ Under Development *****************
     */ 
    void writeAdiosFile();
};

// Free functions for writing output files

/*
 * Function to write vtk mesh from omegah mesh for individual plane.
 * Omega_h::Mesh mesh (in): input Omegah mesh for the plane.
 * int planeNum (in): Plane number of the poloidal plane. Needed only for naming
 * 		      output vtk files.
 */
void writeOmegah2Vtk(Omega_h::Mesh mesh, int planeNum);

/*
 *  Function to write vtk files from a vector omegah meshes.
 *  const std::vector <Omega_h::Mesh>& omegahMeshPlanes (in): a vector of omegah meshes.
 */
void writeVtkFromOmegah(const std::vector <Omega_h::Mesh>& omegahMeshPlanes);

/*
 *  Function to write gmsh files from a vector omegah meshes.
 *  const std::vector <Omega_h::Mesh>& omegahMeshPlanes (in): a vector of omegah meshes.
 */
void writeGmshFromOmegah(const std::vector <Omega_h::Mesh>& omegahMeshPlanes);

// Debug Functions
void debugMesh(std::vector <pVertex> v, std::vector <pEdge> e,  
          std::vector <pFace> f, std::vector <pRegion> r);

#endif
