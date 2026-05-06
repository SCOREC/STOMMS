find_package(Omega_h REQUIRED)
find_package(Adios2 REQUIRED)

set(XGC_READER_SRC
  src/xgc_reader_model.cpp
  src/xgc_reader_topology.cpp
  src/xgc_reader_classification.cpp
  src/xgc_reader_reverse_classification.cpp
  src/xgc_reader_utility.cpp
  src/xgc_reader_plane.cpp
  src/xgc_reader_nonaligned_vertices.cpp
  src/xgc_reader_interface.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)      # Header files for the code
add_library(xgc_mesh_reader ${XGC_READER_SRC})

# Link dependent libraries and header files
target_link_libraries(xgc_mesh_reader PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
target_link_libraries(xgc_mesh_reader PUBLIC Omega_h::omega_h)
target_link_libraries(xgc_mesh_reader PUBLIC adios2::cxx11)

target_include_directories(xgc_mesh_reader PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_include_directories(xgc_mesh_reader PUBLIC $<INSTALL_INTERFACE:include>)

install(TARGETS xgc_mesh_reader
  EXPORT xgc_mesh_reader_targets 
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include)

set(XGC_READER_INCLUDE
  include/xgc_reader_types.hpp
  include/xgc_reader_model.hpp
  include/xgc_reader_topology.hpp
  include/xgc_reader_classification.hpp
  include/xgc_reader_reverse_classification.hpp
  include/xgc_reader_utility.hpp
  include/xgc_reader_plane.hpp
  include/xgc_reader_nonaligned_vertices.hpp
  include/xgc_reader_interface.hpp
)
install(FILES ${XGC_READER_INCLUDE} DESTINATION include)

# ******************************************
# Write xgc_mesh_reader as a package
# ******************************************

include(CMakePackageConfigHelpers)

# Write the version number
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/Xgc_Mesh_ReaderConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Config 
configure_package_config_file(
 ${CMAKE_SOURCE_DIR}/cmake/Xgc_Mesh_ReaderConfig.cmake.in # input file
 ${CMAKE_CURRENT_BINARY_DIR}/Xgc_Mesh_ReaderConfig.cmake  # output file 
 INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xgc_Mesh_Reader
)

install(
FILES ${CMAKE_CURRENT_BINARY_DIR}/Xgc_Mesh_ReaderConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/Xgc_Mesh_ReaderConfigVersion.cmake
DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xgc_Mesh_Reader
)

install(EXPORT xgc_mesh_reader_targets
        FILE Xgc_Mesh_ReaderTargets.cmake
        NAMESPACE Xgc_Mesh_Reader::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xgc_Mesh_Reader
)

# ******************************************
# Add an executable for test case
# ******************************************
add_executable(xgc_reader_test test/reader.cc)
target_link_libraries(xgc_reader_test PRIVATE xgc_mesh_reader)
target_include_directories(xgc_reader_test PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
install(TARGETS xgc_reader_test 
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
