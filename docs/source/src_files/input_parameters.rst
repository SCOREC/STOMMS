Input Parameters
================

To run ``stomms_meshgen`` to create a mesh from magnetic field information, all the inputs need to be provided in a file called "mesh_input". An example "mesh_input" is presented at the end of the page for the reference. See :ref:`Sample Input File`. The input parameters are briefly discussed in this section.

Parameters
----------

Magnetic Field
^^^^^^^^^^^^^^

Stellarators
~~~~~~~~~~~~
* ``vmecFile``
   * The name of the VMEC file that is used to generate a model of the core region of the stellarator.

Tokamaks
~~~~~~~~
* ``eqdskFile``
   * The name of the EQDSK file that is used to generate a model of the Tokamak cross-section. If not specified, limiter (wall curve) is read and used from this equilibrium file.

* ``reversePsi``
   * 0 (default) : Use psi value from eqdskFile as it is.
   * 1 : Multiply -1 to psi value so that psi at minor axis (O-point) gets minimum value of psi in outboard midplane.


Physical Components
^^^^^^^^^^^^^^^^^^^
The only physical component supported at the moment is limiter (wall curve). As we move forward, we can add new components as needed.

* ``limiterFile``
   * A filename that contains a user-specified wall curve. Used when a complex wall curve causes trouble in meshing or in XGC run.
 
Flux Curves Definition
^^^^^^^^^^^^^^^^^^^^^^
* ``fluxFile`` 
   * An ASCII file containing the number of flux curves on a poloidal plane along with the normalized psi values of the flux curves.
   * If the normalized psi value of the flux curve is greater than 1.0 (last closed flux curve), it will be deleted.
   * The number of flux curves on each plane are going to be same so the file contains the flux information for a single plane.
   * The first line of the file contains the number of flux curves and the next lines contain the normalized psi values of flux curves with one psi value on each line.
   * An example ``fluxFile`` is provided below where total number of flux curves is 11.

.. code-block:: text

  11
  0.0
  0.1
  0.2
  0.3
  0.4
  0.5
  0.6
  0.7
  0.8
  0.9
  1.0

Poloidal Planes Definition
^^^^^^^^^^^^^^^^^^^^^^^^^^
* ``planeFile``
   * An ASCII file containing the number of poloidal planes along with the toroidal angle of each plane.
   * The angle should be in degrees and in the range of 0 - 360.
   * The first line of the file contains the number of poloidal planes and the next lines contain the toroidal angles of the planes.
   * An example ``planeFile`` is provided below where total number of poloidal plane is 7 covering 1/4th of the toroidal circuit.

.. code-block:: text

  7
  0.0
  15.0
  30.0
  45.0
  60.0
  75.0
  90.0

Meshing Parameters
^^^^^^^^^^^^^^^^^^
* ``meshSizeFile``
   * An input file to set the desired number of mesh vertices on each flux curve.
   * The number of mesh vertices on each flux curve will remain same on all the poloidal planes. 
   * The first line of the file provides the total number of flux curves.
   * After that, the first column provides the normalized psi value of curve, and the second column provides desired mesh size (in terms of number mesh vertices) for the corresponding flux curve.
   * An example ``meshSizeFile`` is provided below. In this example, there are 11 flux curves, and the desired number of vertices on flux curves with normalized psi values of 0.2 and 0.6 are 4 and 20 respectively. 

.. code-block:: text

  11
  0.0  1
  0.1  2
  0.2  4
  0.3  8
  0.4  12
  0.5  16
  0.6  20
  0.7  24
  0.8  28
  0.9  32
  1.0  36


.. _sample-input:

Sample Input File
-----------------

.. code-block:: text

  !******************MAGNETIC FIELD SOURCE FILES (VMEC, EQDSK for now)***************

  !STELLARATORS 
  !The name of the vmec file for the core region
  vmecFile wout.nc

  !TOKAMAKS
  !The name of eqdsk file for the tokamak cases
  !eqdskFile g096333.03337 

  !If set to 1, this reverses the sign of Psi in eqdsk file, allowing minimum at axis.
  reversePsi 0

  !******************PHYSICAL COMPONENTS SOURCES (WALL CURVE FOR NOW)*****************
  !limiterFile limiter.txt

  !*******************************RESOLUTION SETTINGS*********************************
  !The file containing the number and normalized psi values of flux curves
  fluxFile flux.txt 

  !The file containing the number and toroidal angles of the desired poloidal planes
  planeFile plane.txt

  !Set the mesh size from the input file
  meshSizeFile mesh_size.txt

