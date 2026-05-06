#!/bin/bash

# Set the targets that need to be compiled.
COMPILE_STOMMS_MESHGEN="ON"
COMPILE_STOMMS_READER="ON"

# Set common variables
OMEGAH_DIR=/p/epsi/rpi/install/OmegaH/2026_04_28
ADIOS2_DIR=/p/xgc/Software/install/adios2/5.14.0-570.21.1.el9_6.x86_64/gcc-13.2.0
PREFIX=/p/epsi/rpi/install/STOMMS/2026_04_28
CMAKETYPE=Debug

# If stomms_meshgen is ON, we need all the variables, if only stomms_reader is ON, 
# then we only need adios2 and omegah dependencies. 
if [[ "$COMPILE_STOMMS_MESHGEN" == "ON" ]]; then
  SIM_VER=2026.0-260404
  SIM_ARCHOS=x64_rhel8_gcc83
  SIM_DIR=/opt/hpc/software/Simmetrix/simmodsuite
  PSPLINE_DIR=/p/epsi/rpi/install/PSPLINE/2026_05_05/gcc13.2.0-openmpi4.1.6
  NETCDF_CXX4_DIR=/opt/flux/software/spack/linux-rocky9-zen4/gcc-13.2.0/netcdf-cxx4-4.3.1-k3f2tuicxxnld7ncdhztay3nnpfpw5si
  cmake .. \
    -DCMAKE_C_COMPILER=mpicc \
    -DCMAKE_CXX_COMPILER=mpicxx \
    -DCMAKE_Fortran_COMPILER=mpif90 \
    -DCMAKE_C_FLAGS="-g -O0" \
    -DCMAKE_CXX_FLAGS="-g -O0" \
    -DCMAKE_Fortran_FLAGS="-fpic -ffree-line-length-0"\
    -DSIMMETRIX_INCLUDE_DIR=$SIM_DIR/$SIM_VER/include \
    -DSIMMETRIX_LIB_DIR=$SIM_DIR/$SIM_VER/lib/$SIM_ARCHOS \
    -DPSPLINE_INCLUDE_DIR=$PSPLINE_DIR/mod \
    -DPSPLINE_LIB_DIR=$PSPLINE_DIR/lib \
    -DOmega_h_DIR=$OMEGAH_DIR/lib64/cmake/Omega_h \
    -DAdios2_DIR=$ADIOS2_DIR/lib64/cmake/adios2 \
    -DNETCDF_CXX4_LIB_DIR=$NETCDF_CXX4_DIR/lib64 \
    -DNETCDF_CXX4_INCLUDE_DIR=$NETCDF_CXX4_DIR/include \
    -DENABLE_STOMMS_MESHGEN=$COMPILE_STOMMS_MESHGEN \
    -DENABLE_STOMMS_READER=$COMPILE_STOMMS_READER \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DENABLE_TESTING=ON \
    -DENABLE_PPPL=ON \
    -DCMAKE_BUILD_TYPE=$CMAKETYPE
elif [[ "$COMPILE_STOMMS_MESHGEN" == "OFF" && "$COMPILE_STOMMS_READER" == "ON" ]]; then 
  cmake .. \
    -DCMAKE_C_COMPILER=mpicc \
    -DCMAKE_CXX_COMPILER=mpicxx \
    -DCMAKE_C_FLAGS="-g -O0" \
    -DCMAKE_CXX_FLAGS="-g -O0" \
    -DOmega_h_DIR=$OMEGAH_DIR/lib64/cmake/Omega_h \
    -DAdios2_DIR=$ADIOS2_DIR/lib64/cmake/adios2 \
    -DENABLE_STOMMS_MESHGEN=$COMPILE_STOMMS_MESHGEN \
    -DENABLE_STOMMS_READER=$COMPILE_STOMMS_READER \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_BUILD_TYPE=$CMAKETYPE
fi
