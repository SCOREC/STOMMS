# STOMMS Documentation

## Required modules

- doxygen

On SCOREC machines, if doygen is not loaded already, load it:

```
module use /opt/scorec/spack/rhel9/v0201_4/lmod/linux-rhel9-x86_64/Core/
module load gcc/12.3.0-iil3lno doxygen/1.9.6-5ppxahb
```

In STOMMS/docs, run runDocScript.sh to generate html files in STOMMS/docs/build.
Make sure to first configure CMake variables by running ../config-files/system.sh.
For example on SCOREC machine.

```
cd STOMMS
mkdir build
cd build
../config-files/scorec_rhel9_config.sh
cd ../docs
./runDocScript.sh
```

