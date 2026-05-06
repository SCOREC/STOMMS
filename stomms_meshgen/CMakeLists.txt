find_package(SimModSuite) #modify to take argForDebug and internal flags
find_package(PSPLINE REQUIRED)
find_package(Omega_h REQUIRED)
find_package(Adios2 REQUIRED)
find_package(PkgConfig REQUIRED)

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
  # Workflow
  src/stomms.cc

  # Inputs
  src/input.cc

  # EQDSK
  src/criticalPointsEqdsk.cc 
  src/eqdskData.cc
  src/fluxCurveUtilityEqdsk.cc 
  src/genFluxCurvesEqdsk.cc
  src/magneticGeometryTokamak.cc
  src/modelDataEqdsk.cc
  src/modelingEqdsk.cc

  # VMEC
  src/modelDataVmec.cc
  src/modelingVmec.cc
  src/magneticGeometryStellarator.cc

  # Geometry(Physics & Physical) 
  src/criticalPoints.cc
  src/fieldPointsOnFlux.cc
  src/magneticGeometry.cc  
  src/magneticGeometryDataTypes.cc
  src/modelPhysicsQueries.cc
  src/physicalGeometry.cc
  src/physicsAttributes.cc
  src/plane.cc

  # General Modeling (not specific to input type)
  src/modelMetaData.cc
  src/modelTopology.cc
  src/stommsModel.cc
  
  # Meshing
  src/meshMetaData.cc
  src/stommsMesh.cc

  # Output
  src/output.cc  
  src/meshOutput.cc

  # Utilitis
  src/util.cc 
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
   ${PSPLINE_LIBRARIES}
   Omega_h::omega_h
   adios2::cxx11
)

# NetCDF (C, Cxx4) doesn't have cmake targets at PPPL so need to do
# this step manually. Might clean it in future.
option(ENABLE_PPPL OFF)
message(STATUS "PPPL:  ${ENABLE_PPPL}")
if (ENABLE_PPPL)
  pkg_check_modules(netcdf REQUIRED IMPORTED_TARGET netcdf) 
  target_include_directories(stommsLib PUBLIC ${NETCDF_CXX4_INCLUDE_DIR})
  target_link_directories(stommsLib PUBLIC ${NETCDF_CXX4_LIB_DIR})
  target_link_libraries(stommsLib PUBLIC netcdf-cxx4)
  target_link_libraries(stommsLib PUBLIC PkgConfig::netcdf)
  target_compile_definitions(stommsLib PRIVATE ENABLE_PPPL)
else()
  pkg_check_modules(netcdf-cxx4 REQUIRED IMPORTED_TARGET netcdf-cxx4)
  target_link_libraries(stommsLib PUBLIC PkgConfig::netcdf-cxx4) 
endif()

# Finally add executable
add_executable(stomms_meshgen src/main.cc)
target_link_libraries(stomms_meshgen PRIVATE stommsLib)

# Install the target
install(TARGETS stomms_meshgen
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
