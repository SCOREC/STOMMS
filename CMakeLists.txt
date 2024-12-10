cmake_minimum_required(VERSION 3.9...3.18)
project(STOMMS VERSION 1.0.0 LANGUAGES C CXX Fortran)

include(GNUInstallDirs)


# Let CMake know where to find custom FindFoo.cmake files
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(MPI COMPONENTS CXX REQUIRED)
find_package(OpenMP COMPONENTS CXX REQUIRED)

find_package(SimModSuite) #modify to take argForDebug and internal flags
find_package(PkgConfig REQUIRED)
pkg_check_modules(netcdf-cxx4 REQUIRED IMPORTED_TARGET netcdf-cxx4) 

set(STELLMESH_SRCS
  src/main.cc
  src/input.cc 
  src/magneticGeometry.cc
  src/util.cc 
  src/stomms.cc
  src/modelTopology.cc
  src/modelMetaData.cc
  src/modeling.cc 
  src/modelData.cc
  src/meshMetaData.cc
  src/stommsMesh.cc
)

add_executable(stell_meshgen ${STELLMESH_SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)        # Header files for the code
target_link_libraries(stell_meshgen PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
target_link_libraries(stell_meshgen PRIVATE ${SIMMODSUITE_LIBS})
target_link_libraries(stell_meshgen PRIVATE PkgConfig::netcdf-cxx4)
target_include_directories(stell_meshgen PRIVATE ${SIMMODSUITE_INCLUDE_DIR})

install(TARGETS stell_meshgen
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

