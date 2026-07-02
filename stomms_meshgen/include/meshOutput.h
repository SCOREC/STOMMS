#ifndef MESHOUTPUT_H
#define MESHOUTPUT_H

#include "stommsMesh.h"
#include <adios2.h>
#include "Omega_h_build.hpp"
#include "Omega_h_mesh.hpp"
#include "Omega_h_file.hpp"
#include "Omega_h_adios2.hpp"

//==========================================
// Structs
//==========================================

/**
 * A struct to contain all the flux curves that
 * belong to particular group (closed, open etc.)
 */
struct CurvesGroup {
  std::vector <int> flxId;  // ids of flux curves
  std::vector <double> psi;  // psi values of flux curves
  std::vector <int> modelEdgesVector;  // vector containing model edges on each flux curve
  std::vector <int> modelEdgesRange;  // range of entities in above vector
};

/**
 * A struct to store all the curves of one type and related physics and model 
 * information. The following data is stored.
 * flxId {x1,x2,x3,x4,....,xn}
 * modelEdgesVector {y1,y2,y3,y4,y5,.....,yn}
 * modelEdgesRange {z1,z2,z3,z3,....,zn}
 * For flux of id x1, the model edges on it are in modelEdgesVector from
 * index = z1 to index = z2-1, similarly, for x2, from index = z2, to
 * index = z3-1 ......
 */
struct Adj {
  std::vector <int> entId;  // ids of model entities
  std::vector <int> adjVector_1;  // all adjacent model entities of first of two dimensions.
  std::vector <int> rangeVector_1;  // range of entities in above vector
  std::vector <int> adjVector_2; // all adjacent model entities of second of two dimensions.
  std::vector <int> rangeVector_2;  // range for above vector
};

/**
 * A struct to have an operator compare flux curve value. 
 */
struct FluxCompare {
  bool operator()(const Flux& flux1, const Flux& flux2) const
  {
    const double epsilon = 1e-8;
    return flux1.psiNormOnFlux < flux2.psiNormOnFlux - epsilon;
  }
};

//==========================================
// Model Classification Fucntions
//==========================================
/**
 * Function to get a map between physics classification (int) and list of tags of model entities classified
 * on them. int = 0-7.
 * @param modelFaces: a vector of model faces which need classification.
 * @return a map between face physics id and list of tags of model entities on it.
 */
std::map <int, std::vector <int>> modelFaceClassification(const std::vector <Face>& modelFaces);

/**
 * Function to get a map between physics classification (int) and list of tags of model entities classified
 * on them. int = 0-2 (xPoint, oPoint, none).
 * @param oPoint: model vertex classified on oPoint.
 * @param xPoints: a vector of x-point model vertices.
 * @return a map between physics id and list of tags of model entities on it.
 */
std::map <int, std::vector <int>> modelVertexClassification(Vertex oPoint, std::vector <Vertex> xPoints);

/**
 * Function to arrange and index the curves as needed in XGC.
 * @param fluxCurves: a vector of unsorted flux curves.
 * @return list of flux curves sorted in term of psi values.  
 */
std::vector<Flux> curveIndexing(const std::vector<Flux>& fluxCurves);

/**
 * Function to return a map with type of curve as key (closed, open, separatrix, wall) and 
 * list of curves in that particular group (stored in class CurveGroup).
 * @param fluxCurves: a vector of sorted flux curves (sorted in terms of psi values).
 * @param wallCurve: wall curve ( a vector of model edges on wall curve).
 * @return a map between curve type and set of curve with that type in CurveGroup.
 */
std::map <std::string, CurvesGroup> curveClassification(const std::vector <Flux>& fluxCurves, const std::vector<Edge>& wallCurve);

/**
 * Function to return a vector of tags of model edges that are classified on a curve.
 * @param curve: curve on which model edges tags are needed.
 * @return a vector of tags of model edges classified on curve.
 */
std::vector <int> getModelEdgesTagsOnCurve(const Flux& curve);

/**
 * Given the physics type id of model face, returns the name of physics property.
 * @param faceId: physics Id (physics region type) of the model face.
 * @return a string with property name.
 */
std::string getFaceType(int faceId);

/**
 * Given the physics type id of model edge, returns the name of physics property.
 * @param edgeId: physics Id (curve type) of the model edge.
 * @return a string with property name.
 */
std::string getEdgeType(int edgeId);

/**
 * Given the physics type id of model vertex, returns the name of physics property.
 * @param vertexId: physics Id (critical point type) of the model vertex.
 * @return a string with property name.
 */
std::string getVertexType(int vertexId);

//==========================================
// Model Adjacency Functions
//==========================================
/*
 * Given the dimension of entity (0,1,2), return Adj of all the model entities with
 * that dimension on model.
 * int inDim (in): The input dimension. Forexample inDim = 0 means to return adjacencies
 *                 on all the model vertices in the model.
 * pGModel& model (in): Simmetrix model.
 * returns Adj (see details above)
 */ 
Adj getAdjacency(int inDim, const Plane& plane);

/**
 * Given a vector of model vertices, return both edge and face adjacencies on 
 * all the model vertices.
 * @param vertices: a vector of model vertices.
 * @return Adj
 */
Adj getVertexAdj(const std::vector <Vertex>& vertices);

/**
 * Given a vector of model edges, return both vertex and face adjacencies on 
 * all the model edges.
 * @param edges: a vector of model edges.
 * @return Adj
 */
Adj getEdgeAdj(const std::vector <Edge>& edges);

/**
 * Given a vector of model faces, return both vertex and edge adjacencies on 
 * all the model faces.
 * @param faces: a vector of model faces.
 * @return Adj
 */
Adj getFaceAdj(const std::vector <Face>& faces);

//==========================================
// Templated Fucntions
//==========================================
/**
 * Function to write an array(2D) in adios2 file.
 * @param io: adios2 IO.
 * @param writer: adios2 write engine.
 * @param arrayData: array data in vector.
 * @param ncomp: Number of components. The output array
 *               size = arrayData.size()/ncomp
 * @param name: output array name.
 */
template <typename T>
void writeAdios2Array(adios2::IO& io, adios2::Engine& writer, 
          std::vector <T> arrayData, int ncomp, std::string& name)
{
  // Step 1: Check array size and return if its empty.
  if (arrayData.size() == 0)
    return;
 
  const std::size_t Nx = arrayData.size()/ncomp;

  // To avoid narrowing conversion warning.
  size_t n_comp = static_cast <size_t>(ncomp);

  // Step 2: Write data to adios2 variable
  adios2::Variable<T> bpData =  io.DefineVariable<T>(name, {Nx, n_comp}, {0,0}, {Nx, n_comp}, adios2::ConstantDims);
  writer.Put(bpData, arrayData.data());
}

/**
 * Function to write a value in adios2 file.
 * @param io: adios2 IO.
 * @param writer: adios2 write engine.
 * @param value: value to be written to adios2 file.
 * @param name: output variable name in adios2 file.
 */
template <typename T>
void writeAdios2Value(adios2::IO& io, adios2::Engine& writer,
                     T value, std::string& name)
{
  adios2::Variable <T> val = io.DefineVariable <T> (name);
  writer.Put(val, value);
}

template <typename T>
void writeAdios2MultiDimArray(adios2::IO& io, adios2::Engine& writer,
          std::vector <T>& arrayData, std::vector <int>& arrayShape, std::string& name)
{
  // step 1: Safety checks before proceeding. Make sure array has the correct size.
  int desiredSize = 1;
  for (int i = 0; i < arrayShape.size(); i++)
    desiredSize = desiredSize*arrayShape[i];

  if (arrayData.size() == 0)
    return;  

  if (arrayData.size() != desiredSize)
  {
    std::cout << "The given array data size is " << arrayData.size() << " which doesn't \n match the size of given array shape\n"; 
    exit(1);
  }
  
  // Step 2: If data is correct, create ADIOS2 array shape, and count.
  adios2::Dims shape(arrayShape.begin(), arrayShape.end());
  adios2::Dims start(arrayShape.size(), 0);
  adios2::Dims count(arrayShape.begin(), arrayShape.end());

  // Step 3: Write the data to the adios2 variable
  adios2::Variable<T> bpData = io.DefineVariable<T>(name, shape, start, count, adios2::ConstantDims);
  writer.Put(bpData, arrayData.data()); 
}
#endif
