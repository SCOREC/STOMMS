#ifndef OUTPUT_H
#define OUTPUT_H

#include "stommsMesh.h"
#include <adios2.h>
#include "Omega_h_build.hpp"
#include "Omega_h_mesh.hpp"
#include "Omega_h_meshsim.hpp"
#include "Omega_h_file.hpp"

class StommsOutput{
  public:
    StommsOutput(const StommsMesh& m);
  private:
    StommsMesh mesh;
    pMesh simMesh;
    pGModel simModel;
    std::vector <PlaneMeshMetaData> planes;
    void writeOmegahMeshes();
    void writeAdiosFile();
    Omega_h::Mesh simMesh2Omegah(const PlaneMeshMetaData& p);
};

#endif
