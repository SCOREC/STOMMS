cmake_minimum_required(VERSION 3.9...3.18)
project(STOMMS VERSION 1.0.0 LANGUAGES C CXX Fortran)

include(GNUInstallDirs)

# Let CMake know where to find custom FindFoo.cmake files
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Find the common packages needed.
find_package(MPI COMPONENTS CXX REQUIRED)
find_package(OpenMP COMPONENTS CXX REQUIRED)

# Optional target building.
option(ENABLE_STOMMS_MESHGEN "Enable Stomms Mesh Generation Tools" OFF)
option(ENABLE_STOMMS_READER "Enable Stomms Mesh Reader" OFF)

message (STATUS "ENABLE_STOMMS_MESHGEN:  ${ENABLE_STOMMS_MESHGEN} ")
message (STATUS "ENABLE_STOMMS_READER:  ${ENABLE_STOMMS_READER} ")

# Build stomms_meshgen
if (ENABLE_STOMMS_MESHGEN)
  message(STATUS "Compiling stomms_meshgen")
  add_subdirectory(stomms_meshgen)
endif()

# Build stomms_reader
if (ENABLE_STOMMS_READER)
  message(STATUS "Compiling stomms_reader")
  add_subdirectory(stomms_reader)
endif()

# If both are not selected, issue a warning
if (NOT ENABLE_STOMMS_MESHGEN AND NOT ENABLE_STOMMS_READER)
  message(STATUS "Warning: No Compilation Target")
endif()

# Add Testing for stomms_meshgen
# Enable testing if its set to ON in config file
option(ENABLE_TESTING "Build for CTest" OFF)
message(STATUS "ENABLE_TESTING: ${ENABLE_TESTING}")

if(ENABLE_TESTING)
  set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stomms_meshgen/testCases" CACHE STRING "Directory of test cases")
  message(STATUS "TEST_DIR: ${TEST_DIR}")

  enable_testing()
  add_subdirectory(stomms_meshgen/test/unitTests)
  add_subdirectory(stomms_meshgen/test/test)
endif() 
