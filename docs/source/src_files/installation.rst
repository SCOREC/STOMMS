Installation and Testing
========================

Installation
------------
STOMMS has two compilation targets:
  1. ``stomms_meshgen``
  2. ``stomms_reader``

``stomms_meshgen`` is an executable to generate tokamak and stellarator models and meshes. ``stomms_reader`` is a library to read the STOMMS meshes. It contains a set of API's to read the mesh data from stomms output. The dependencies for these targets are:

  1. Simmetrix Simmodsuite (for ``stomms_meshgen``)
  2. PSPLINE (for ``stomms_meshgen``)
  3. Omega_h (for both ``stomms_meshgen`` and ``stomms_reader``)
  4. Adios2 (for both ``stomms_meshgen`` and ``stomms_reader``) 

The installation instructions for each system are provided here.

SCOREC Rhel9
^^^^^^^^^^^^

Required Modules and Libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   module use /opt/scorec/spack/rhel9/v0201_4/lmod/linux-rhel9-x86_64/Core/
   module load zlib/1.2.13-mvjz5oi gcc/12.3.0-iil3lno mpich/4.1.1-xpoyz4t cmake
   module load simmetrix-simmodsuite/2025.1-250602dev-yv5oiom netcdf-c/4.9.2-2ilqxr3 netcdf-cxx4/4.3.1-7427fnc
   module load simmetrix/simModeler/2025.0-250108-dev
   module load adios2/2.9.0-nztsv7o
   export LD_LIBRARY_PATH=/opt/scorec/spack/rhel9/v0201_4/install/linux-rhel9-x86_64/gcc-12.3.0/netcdf-c-4.9.2-2ilqxr3rpuva5xlwzkzeqitei4oybuba/lib:$LD_LIBRARY_PATH
   export LD_LIBRARY_PATH=/opt/scorec/spack/rhel9/v0201_4/install/linux-rhel9-x86_64/gcc-12.3.0/netcdf-cxx4-4.3.1-7427fncoukmgmapu2ylfyuhgnawm77og/lib:$LD_LIBRARY_PATH
   export LD_LIBRARY_PATH=/opt/scorec/spack/rhel9/v0201_4/install/linux-rhel9-x86_64/gcc-12.3.0/adios2-2.9.0-nztsv7oahftcta2dntesql2wkep6kdfp/lib64:$LD_LIBRARY_PATH
   export LD_LIBRARY_PATH=/lore/riazu2/Tools/install/OmegaH/gcc12.3.0/lib64:$LD_LIBRARY_PATH

Compilation
~~~~~~~~~~~

.. code-block:: bash

   - git clone https://github.com/SCOREC/STOMMS.git
   - cd STOMMS
   - Edit config-files/scorec_rhel9_config.sh as needed (at minimum need to change the PREFIX).
   - Edit PREFIX in the scorec_rhel9_config.sh and set it to the location where you want to install executable/library.
   - Set COMPILE_STOMMS_MESHGEN="ON" to compile stomms_meshgen and set COMPILE_STOMMS_READER="ON" to install reader
     library. By default both are "ON" on SCOREC machines. Set "OFF" if they are not needed. 
   - mkdir build
   - cd build
   - ../config-files/scorec_rhel9_config.sh
   - make install

PPPL Flux
^^^^^^^^^
STOMMS meshing executables ``stomms_meshgen`` and required modules ``env_stomms.sh`` can be found in the directory below. They are ogranized by date in the format ``YYYY_MM_DD``. 

.. code-block:: text
   
   /p/epsi/rpi/install/STOMMS/YYYY_MM_DD/bin/stomms_meshgen
   /p/epsi/rpi/install/STOMMS/YYYY_MM_DD/modules/env_stomms.sh

For example, the latest one is:

.. code-block:: text

   /p/epsi/rpi/install/STOMMS/2026_04_28/bin/stomms_meshgen
   /p/epsi/rpi/install/STOMMS/2026_04_28/modules/env_stomms.sh

To install ``stomms_meshgen``, follow the instruction below.

Required Modules and Libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   module use /opt/scorec/spack/rhel9/v0201_4/lmod/linux-rhel9-x86_64/Core/
   module purge
   module load gcc/13.2.0
   module load openmpi/4.1.6-gcc-13.2.0
   module load openmpi-4.1.6/gcc-13.2.0/netcdf-c/main-4adke
   module load openmpi-4.1.6/gcc-13.2.0/netcdf-cxx4/4.3.1-k3f2t
   module load simmodsuite/2026.0-260404
   setenv LD_LIBRARY_PATH /p/epsi/rpi/install/OmegaH/2026_04_28/lib64:$LD_LIBRARY_PATH
   setenv LD_LIBRARY_PATH /p/xgc/Software/install/adios2/5.14.0-570.21.1.el9_6.x86_64/gcc-13.2.0/lib64:$LD_LIBRARY_PATH
   setenv OMP_NUM_THREADS 1

Compilation
~~~~~~~~~~~

.. code-block:: bash

   - git clone https://github.com/SCOREC/STOMMS.git
   - cd STOMMS
   - Edit config-files/pppl_flux_config.sh as needed (at minimum need to change the PREFIX).
   - Edit PREFIX in the pppl_flux_config.sh and set it to the location where you want to install executable/library.
   - Set COMPILE_STOMMS_MESHGEN="ON" to compile stomms_meshgen and set COMPILE_STOMMS_READER="ON" to install reader
     library. By default both are "ON" on PPPL Flux. Set "OFF" if they are not needed. 
   - mkdir build
   - cd build
   - ../config-files/pppl_flux_config.sh
   - make install

Perlmutter
^^^^^^^^^^
Perlmutter is mainly used to run the analysis codes, therefore mesh generation libraries and Simmetrix Simmodsuite tools are not available at Perlmutter. Since Perlmuuter does not have Simmetrix libraries, ``stomms_meshgen`` is not supported and only ``stomms_reader`` can be compiled at Perlmutter.

- ADD INSTRUCTIONS HERE LATER...

Execution
---------
To run ``stomms_meshgen``, make sure input file ``mesh_input`` is provided with a set of required input files. For details of the inputs files along with the set of the 
input paramters, check :doc:`input_parameters`. 

.. code-block:: bash

   /path_to_prefix/bin/stomms_meshgen

Testing
-------
As STOMMS is under continuous development, we will keep adding new test cases as we move forward. To run the CMake Testing ``ctest``,
follow these steps:

.. code-block:: bash

  - set DENABLE_TESTING=ON in config files.
  - In STOMMS/build, run ctest
