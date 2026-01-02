General Information
===================

Stellarator and Tokamak Modeling and Meshing Software (STOMMS) is a software designed for the generation of field-aligned meshes for both stellarators and tokamaks.
This software contains new developments for stellarator meshing, along with the integration of methods from Tokamak Modeling and Meshing Software (TOMMS) for tokamaks.
(Add citation for the papers here to TOMMS). Morever, STOMMS has a mesh reader interface to load and read STOMMS meshes written in adios2 file format. The reader provides a set of API's to read model topology, mesh classification, reverse classification, and physics classification information from adios2 file. 

STOMMS supports following magnetic field files in its current implementation.

1. Variational Moments Equilibrium Code (VMEC) for stellarators (only core region)
2. EQDSK for tokamaks (full domain support)

The current focus of the development is the implementation of the methods from TOMMS. The current and future tasks include:

1. Update STOMMS to fully support tokamak meshes.
2. Support outside the last closed flux surface region in stellarators.


