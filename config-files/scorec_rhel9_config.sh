SIM_VER=simmetrix-simmodsuite-2024.1-240902dev-7pk4foxpfdrqnwj66qcagcgptxmirib4
SIM_ARCHOS=x64_rhel8_gcc83
PREFIX=/lore/riazu2/STOMMS_dev/STOMMS/install
CMAKETYPE=Debug
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
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DCMAKE_BUILD_TYPE=$CMAKETYPE
