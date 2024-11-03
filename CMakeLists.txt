cmake_minimum_required(VERSION 3.9...3.18)
project(STOMMS VERSION 1.0.0 LANGUAGES C CXX Fortran)

include(GNUInstallDirs)


# Let CMake know where to find custom FindFoo.cmake files
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(MPI COMPONENTS CXX REQUIRED)
find_package(OpenMP COMPONENTS CXX REQUIRED)

find_package(SimModSuite) #modify to take argForDebug and internal flags
find_package(PkgConfig REQUIRED)
pkg_check_modules(netcdf-cxx4 REQUIRED IMPORTED_TARGET netcdf-cxx4) 

find_package(pybind11 REQUIRED)
set(CMAKE_CXX_VISIBILITY_PRESET "default")
pybind11_add_module(py_stell src/python_bindings.cpp src/input.cc)

set(STELLMESH_SRCS
  src/main.cc
  src/model.cc 
  src/input.cc
  src/util.cc
  src/meshing.cc
  src/modelTopology.cc
)

add_executable(stell_meshgen ${STELLMESH_SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)        # Header files for the code
target_link_libraries(stell_meshgen PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
target_link_libraries(stell_meshgen PRIVATE ${SIMMODSUITE_LIBS})
target_link_libraries(stell_meshgen PRIVATE PkgConfig::netcdf-cxx4)
target_include_directories(stell_meshgen PRIVATE ${SIMMODSUITE_INCLUDE_DIR})

target_link_libraries(py_stell PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX PkgConfig::netcdf-cxx4)
target_link_libraries(py_stell PRIVATE ${SIMMODSUITE_LIBS})
target_include_directories(py_stell PRIVATE ${SIMMODSUITE_INCLUDE_DIR})

set_target_properties(py_stell PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS stell_meshgen
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(TARGETS py_stell
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
