find_package(SimModSuite) #modify to take argForDebug and internal flags
find_package(PSPLINE REQUIRED)
find_package(Omega_h REQUIRED)
find_package(Adios2 REQUIRED)
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
  message(STATUS "Git Version: ${STOMMS_GIT_HASH}")
else()
  message(STATUS "-- Warning: GIT_HASH not found")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/stommsVersion.h.in" "${CMAKE_INSTALL_INCLUDEDIR}/stommsVersion.h" @ONLY)

# set sources files in STELLMESH_SRCS
set(STELLMESH_SRCS
  src/input.cc
  src/physicalGeometry.cc
  src/criticalPoints.cc
  src/criticalPointsEqdsk.cc
  src/magneticGeometryDataTypes.cc 
  src/magneticGeometry.cc
  src/magneticGeometryStellarator.cc
  src/magneticGeometryTokamak.cc
  src/util.cc 
  src/stomms.cc
  src/modelTopology.cc
  src/modelMetaData.cc
  src/modelingVmec.cc 
  src/modelDataVmec.cc
  src/stommsModel.cc
  src/meshMetaData.cc
  src/stommsMesh.cc
  src/output.cc
  src/ezfit.F90
)

# Add library
add_library(stommsLib ${STELLMESH_SRCS})

# Link dependent libraries and header files
target_include_directories(stommsLib PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include  # Header files for the code
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}  # for stommsVersion.h header file
  ${SIMMODSUITE_INCLUDE_DIR}
  ${PSPLINE_INCLUDE_DIR}
)

target_link_libraries(stommsLib PUBLIC 
   MPI::MPI_CXX OpenMP::OpenMP_CXX
   ${SIMMODSUITE_LIBS}
   PkgConfig::netcdf-cxx4
   ${PSPLINE_LIBRARIES}
   Omega_h::omega_h
   adios2::cxx11
)
                                           
# Finally add executable
add_executable(stomms_meshgen src/main.cc)
target_link_libraries(stomms_meshgen PRIVATE stommsLib)

# Install the target
install(TARGETS stomms_meshgen
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
