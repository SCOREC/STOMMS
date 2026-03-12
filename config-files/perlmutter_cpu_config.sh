#!/bin/bash
# Set the targets that need to be compiled.
# Simmetrix tools aren't available on Perlmutter, so meshgen isn't available here. 
# Compile only STOMMS_READER

COMPILE_STOMMS_MESHGEN="OFF"
COMPILE_STOMMS_READER="ON"

OMEGAH_DIR=/global/common/software/m499/perlmutter/install/omegah/omegah-11.0.2/n9_cpu-gcc-native14-cuda12.9
ADIOS2_DIR=/global/common/software/m499/perlmutter/install/adios2/adios2-2.11.0/n9_cpu-gcc-native14/lib64/cmake/adios2
PREFIX=/global/common/software/m499/perlmutter/install/xgc_mesh_reader/xgc_mesh_reader-1.0.0/n9_cpu-gcc-native14-cuda12.9
CMAKETYPE=Debug
cmake .. \
  -DCMAKE_C_COMPILER=mpicc \
  -DCMAKE_CXX_COMPILER=mpicxx \
  -DCMAKE_C_FLAGS="-g -O0" \
  -DCMAKE_CXX_FLAGS="-g -O0" \
  -DOmega_h_DIR=$OMEGAH_DIR/lib64/cmake/Omega_h \
  -DADIOS2_DIR=$ADIOS2_DIR/ \
  -DENABLE_STOMMS_MESHGEN=$COMPILE_STOMMS_MESHGEN \
  -DENABLE_STOMMS_READER=$COMPILE_STOMMS_READER \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DCMAKE_BUILD_TYPE=$CMAKETYPE
