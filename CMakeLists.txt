cmake_minimum_required(VERSION 3.9...3.18)
project(STOMMS VERSION 1.0.0 LANGUAGES C CXX Fortran)

include(GNUInstallDirs)

# Let CMake know where to find custom FindFoo.cmake files
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Find the packages needed
find_package(MPI COMPONENTS CXX REQUIRED)
find_package(OpenMP COMPONENTS CXX REQUIRED)

find_package(SimModSuite) #modify to take argForDebug and internal flags
find_package(PSPLINE REQUIRED)
find_package(Omega_h REQUIRED)
find_package(ADIOS2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(netcdf-cxx4 REQUIRED IMPORTED_TARGET netcdf-cxx4) 

# Find git hash and save it to GIT_HASH variable
# Issue a warning if can't find git hash
execute_process(
  COMMAND git rev-parse HEAD
  RESULT_VARIABLE gitHashFound  # returns 0 for success, non-zero for an error
  OUTPUT_VARIABLE STOMMS_GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE  # Flag to remove trailing white spaces from the output
)

if (gitHashFound EQUAL 0)
  message("-- Git Version: ${STOMMS_GIT_HASH}")
else()
  message("-- Warning: GIT_HASH not found")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/stommsVersion.h.in" "${CMAKE_INSTALL_INCLUDEDIR}/stommsVersion.h" @ONLY)

# set sources files in STELLMESH_SRCS
set(STELLMESH_SRCS
  src/main.cc
  src/input.cc
  src/physicalGeometry.cc
  src/criticalPoints.cc 
  src/magneticGeometry.cc
  src/util.cc 
  src/stomms.cc
  src/modelTopology.cc
  src/modelMetaData.cc
  src/modeling.cc 
  src/modelData.cc
  src/meshMetaData.cc
  src/stommsMesh.cc
  src/output.cc
  src/ezfit.F90
)

# Add executables and include the directory containing header files
add_executable(stell_meshgen ${STELLMESH_SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)        # Header files for the code
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR})  # for stommsVersion.h header file.

# Link dependent libraries and header files
target_link_libraries(stell_meshgen PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
target_link_libraries(stell_meshgen PRIVATE ${SIMMODSUITE_LIBS})
target_link_libraries(stell_meshgen PRIVATE PkgConfig::netcdf-cxx4)
target_link_libraries(stell_meshgen PRIVATE ${PSPLINE_LIBRARIES})
target_link_libraries(stell_meshgen PRIVATE Omega_h::omega_h)
target_link_libraries(stell_meshgen PRIVATE ${ADIOS2_LIB_DIR}/libadios2_cxx11.so)

target_include_directories(stell_meshgen PRIVATE ${SIMMODSUITE_INCLUDE_DIR})
target_include_directories(stell_meshgen PRIVATE ${PSPLINE_INCLUDE_DIR})
target_include_directories(stell_meshgen PRIVATE ${ADIOS2_INCLUDE_DIR})

# Install the target
install(TARGETS stell_meshgen
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
