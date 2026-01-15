#!/bin/bash

# Set the targets that need to be compiled.
COMPILE_STOMMS_MESHGEN="ON"
COMPILE_STOMMS_READER="OFF"

# Set common variables
OMEGAH_DIR=/users/riazu2/lore.scorec.rpi.edu/Tools/install/OmegaH/gcc12.3.0
ADIOS_DIR=/opt/scorec/spack/rhel9/v0201_4/install/linux-rhel9-x86_64/gcc-12.3.0/adios2-2.9.0-nztsv7oahftcta2dntesql2wkep6kdfp
PREFIX=/users/riazu2/lore.scorec.rpi.edu/STOMMS_dev/STOMMS/install
CMAKETYPE=Debug

# If stomms_meshgen is ON, we need all the variables, if only stomms_reader is ON, 
# then we only need adios2 and omegah dependencies. 
if [[ "$COMPILE_STOMMS_MESHGEN" == "ON" ]]; then
  SIM_VER=simmetrix-simmodsuite-2025.1-250602dev-yv5oiomk2sdcru5cn5vqjxprycytc4ow
  SIM_ARCHOS=x64_rhel9_gcc11
  PSPLINE_DIR=/users/riazu2/lore.scorec.rpi.edu/Tools/install/PSPLINE/gcc12.3.0
  cmake .. \
    -DCMAKE_C_COMPILER=mpicc \
    -DCMAKE_CXX_COMPILER=mpicxx \
    -DCMAKE_Fortran_COMPILER=mpif90 \
    -DCMAKE_C_FLAGS="-g -O0" \
    -DCMAKE_CXX_FLAGS="-g -O0" \
    -DCMAKE_Fortran_FLAGS="-fpic -ffree-line-length-0"\
    -DSIM_MPI=mpich4.1.1 \
    -DSIMMETRIX_INCLUDE_DIR=/opt/scorec/spack/rhel9/v0201_4/install/linux-rhel9-x86_64/gcc-12.3.0/$SIM_VER/include \
    -DSIMMETRIX_LIB_DIR=/opt/scorec/spack/rhel9/v0201_4/install/linux-rhel9-x86_64/gcc-12.3.0/$SIM_VER/lib/$SIM_ARCHOS \
    -DPSPLINE_INCLUDE_DIR=$PSPLINE_DIR/mod \
    -DPSPLINE_LIB_DIR=$PSPLINE_DIR/lib \
    -DOmega_h_DIR=$OMEGAH_DIR/lib64/cmake/Omega_h \
    -DAdios2_DIR=$ADIOS2_DIR/lib64/cmake/adios2 \
    -DENABLE_STOMMS_MESHGEN=$COMPILE_STOMMS_MESHGEN \
    -DENABLE_STOMMS_READER=$COMPILE_STOMMS_READER \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DENABLE_TESTING=OFF \
    -DCMAKE_BUILD_TYPE=$CMAKETYPE
elif [[ "$COMPILE_STOMMS_MESHGEN" == "OFF" && "$COMPILE_STOMMS_READER" == "ON" ]]; then 
  cmake .. \
    -DCMAKE_C_COMPILER=mpicc \
    -DCMAKE_CXX_COMPILER=mpicxx \
    -DCMAKE_C_FLAGS="-g -O0" \
    -DCMAKE_CXX_FLAGS="-g -O0" \
    -DOmega_h_DIR=$OMEGAH_DIR/lib64/cmake/Omega_h \
    -DENABLE_STOMMS_MESHGEN=$COMPILE_STOMMS_MESHGEN \
    -DENABLE_STOMMS_READER=$COMPILE_STOMMS_READER \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_BUILD_TYPE=$CMAKETYPE
fi
