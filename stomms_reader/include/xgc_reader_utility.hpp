#ifndef XGC_READER_UTILITY_H
#define XGC_READER_UTILITY_H

#include "Omega_h_adios2.hpp"
#include "xgc_reader_types.hpp"

/**********************************/
// ADIOS 2Reader Utility Functions
/**********************************/

/*
 * Function to read a scaler value from adios2 file given the variable name.
 * adios2::IO& io (in): adios2 io to interact with adios2 input file.
 * adios2::Engine& reader (in): adios2 reader engine to read data from adios file.
 * T *val (out): a variable to store scaler value in.
 * std::string &name (in): name of variable to read from adios2 file.
 */
template <typename T>
void readAdios2Value(adios2::IO &io, adios2::Engine &reader, T *val, std::string &name)
{
  adios2::Variable<T> bpData = io.InquireVariable<T>(name);
  reader.Get(bpData, val);
}

/*
 * Function to read an array from adios2 file given the variable name.
 * adios2::IO& io (in): adios2 io to interact with adios2 input file.
 * adios2::Engine& reader (in): adios2 reader engine to read data from adios file.
 * std::vector <T> &array (out): an array to store values in.
 * std::string &name (in): name of variable to read from adios2 file.
 */
template <typename T>
void readAdios2Array(adios2::IO &io, adios2::Engine &reader, std::vector <T> &array, std::string &name)
{
  auto var = io.InquireVariable(name);
  std::vector <size_t> shape = var.Count();
  
  size_t arraySize = 1;
  for (size_t size: shape)
    arraySize = arraySize*size;

  array.resize(arraySize);
  adios2::Variable<T> bpData = io.InquireVariable<T>(name);
  if (bpData) // means found
  {
    std::vector <size_t> start(shape.size(),0);
    bpData.SetSelection({start, shape});
    reader.Get(bpData, array.data(),adios2::Mode::Sync);
  }
}

/*
 * Given a group name, returns all the adios2 variables.
 * adios2::IO& io (in): adios2 io to interact with adios2 input file.
 * std::string groupName (in): group name for which variables names are needed.
 */
std::vector <std::string> findVarInGroup(adios2::IO &io, std::string groupName);

/**********************************/
// Other Utility Functions
/**********************************/

/*
 * Given the name of the full varibale (from adios2), extract the model surface type.
 * std::string name (in): name of the variable.
 */
SurfaceType getSurfaceTypeForString(std::string name);

/*
 * Given two points, find the 2D distance between them
 * std::array<double,2>& point1 (in):  Coordinates of point 1.
 * std::array<double,2>& point2 (out): Coordinates of point 2.
 */
double dist2DPointToPoint(const std::array<double,2>& point1, const std::array<double,2>& point2);
#endif
