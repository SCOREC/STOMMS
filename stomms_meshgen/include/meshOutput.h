#ifndef MESHOUTPUT_H
#define MESHOUTPUT_H

#include "stommsMesh.h"
#include <adios2.h>
#include "Omega_h_build.hpp"
#include "Omega_h_mesh.hpp"
#include "Omega_h_file.hpp"
#include "Omega_h_adios2.hpp"

struct CurvesGroup {
  std::vector <int> flxId;  // ids of flux curves
  std::vector <double> psi;  // psi values of flux curves
  std::vector <int> modelEdgesVector;  // vector containing model edges on each flux curve
  std::vector <int> modelEdgesRange;  // range of entities in above vector
};

std::map <int, std::vector <int>> modelFaceClassification(const std::vector <Face>& modelFaces);
std::vector<Flux> curveIndexing(const std::vector<Flux>& fluxCurves);
std::map <std::string, CurvesGroup> curveClassification(const std::vector <Flux>& fluxCurves, const std::vector<Edge>& wallCurve);
std::vector <int> getModelEdgesTagsOnCurve(const Flux& curve);
std::map <int, std::vector <int>> modelVertexClassification(Vertex oPoint, std::vector <Vertex> xPoints);

/*
 * Given the physics type id of model entities (vertex,edge,face), 
 * these functions return the name of physics property.
 * int entId (in): physics Id of the entity.
 */
std::string getFaceType(int faceId);
std::string getEdgeType(int edgeId);
std::string getVertexType(int vertexId);


// Function to write an array(2D) in adios2 file.
template <typename T>
void writeAdios2Array(adios2::IO& io, adios2::Engine& writer, 
          std::vector <T> arrayData, int ncomp, std::string& name)
{
  if (arrayData.size() == 0)
    return;
 
  const std::size_t Nx = arrayData.size()/ncomp;

  // To avoid narrowing conversion warning.
  size_t n_comp = static_cast <size_t>(ncomp);

  adios2::Variable<T> bpData =  io.DefineVariable<T>(name, {Nx, n_comp}, {0,0}, {Nx, n_comp}, adios2::ConstantDims);
  writer.Put(bpData, arrayData.data());
}

// Function to write a value in adios2 file.
template <typename T>
void writeAdios2Value(adios2::IO& io, adios2::Engine& writer,
                     T value, std::string& name)
{
  adios2::Variable <T> val = io.DefineVariable <T> (name);
  writer.Put(val, value);
}
#endif

struct FluxCompare {
  bool operator()(const Flux& flux1, const Flux& flux2) const
  {
    const double epsilon = 1e-8;
    return flux1.psiNormOnFlux < flux2.psiNormOnFlux - epsilon;
  }
};
