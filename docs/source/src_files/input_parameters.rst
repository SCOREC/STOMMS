Input Parameters
================

To run ``stomms_meshgen`` to create a mesh from magnetic field information, all the inputs need to be provided in a file called ``mesh_input``. An example ``mesh_input`` is presented at the end of the page for the reference. See :ref:`Sample Input File`. The input parameters are briefly discussed in this section.

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
   * 0 (default): Use psi value from eqdskFile as it is.
   * 1: Multiply -1 to psi value so that psi at minor axis (O-point) gets minimum value of psi in outboard midplane.

* ``eqdPsiFactor``
   * Multiplication factor to psi values from the given eqd/eqdsk file.
   * default = 1.0

* ``numPlanes``
   * Number of poloidal planes (to be set in XGC). A parameter related to the field-following feature.
   * default = 64

* ``fluxRandomStart``
   * There are two options available to set the starting position of the curves on the outboard/inboard plane for curve generation.
   * 1 (default): Starting points of the curve generation for closed flux surfaces are randomly shifted along the corresponding flux surfaces.
   * 0: Starting points of closed flux surfaces are aligned on a straight line on either the outboard or inboard midplane.

* ``stepRadians``
   * Step size in radian for searching next point for tracing a given flux curve.
   * default = 0.00125

* ``psiTolerance``
   * A tolerance value to guarantee any psi query in the code is within a targeted tolerance value.
   * default = 1E-8

* ``spacingToleranceOptimal``
   * An optimal tolerance value to ensure the target distance between vertices on the curve is acheived or not.
   * default = 0.5

* ``spacingToleranceAbsolute``
   * A strict tolerance value to ensure the target distance between vertices on the curve is acheived or not.
   * default = 0.61803398874989484820

* ``intraCurveSpacingOption``
   * -1 (default): spacing option for the field following nodes on the flux curve.
   * -2: spacing option for the non field following placement of nodes on the flux curves. Uses ``intraCurveSpacingPropFacMax`` and ``intraCurveSpacingPropFacMin``.

* ``intraCurveSpacingPropFacMax``
   * Maximum limit of a proportional factor in the intra-curve spacing function.
   * Used only when ``intraCurveSpacingOption = -2``.
   * default = 1.0

* ``intraCurveSpacingPropFacMin``
   * Minimum limit of a proportional factor in the intra-curve spacing function.
   * Used only when ``intraCurveSpacingOption = -2``.
   * default = 1.0

* ``lastClosedPsi``
   * Only applicable for cases with number of Xpoints = 0. Code doesn't call it if the number of Xpoints are greater than 0.
   * The bounding flux curve of the core region. This is the outermost closed flux curve that will be created in the domain. If the flux curve with ``lastClosedPsi`` intersects with the wall curve (or domain box if there is no wall curve), the code throws an error. The error message indicates that this psi value is too large for closed flux curve, so user can adjust it. 
  
* ``useWall``
   * Only applicable for cases with number of Xpoints = 0. Code doesn't call it if the number of Xpoints are greater than 0.
   * A parameter to set the outer bounds of the domain. In zero Xpoint cases, domain can either be bounded by the flux curve defined by ``lastClosedPsi`` or the wall curve.
   * 1 (default): Domain is bounded by the wall curve in default settings.
   * 0: If wall curve is not desired, and the last closed flux curve is needed as outer bounds, set this option to 0.

-----

Physical Components
^^^^^^^^^^^^^^^^^^^
The only physical component supported at the moment is limiter (wall curve). As we move forward, we can add new components as needed.

* ``limiterFile``
   * A filename that contains a user-specified wall curve. Used when a complex wall curve causes trouble in meshing or in XGC run.
 
-----

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

-----

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

-----

Meshing Parameters
^^^^^^^^^^^^^^^^^^
* ``meshSizeFile``
   * An input file to set the desired node spacing on each flux curve.
   * The number of mesh vertices on each flux curve remain same on all the poloidal planes. The given spacing is used to get the parametric value of nodes on each flux curve on plane 0, and then same parametric values are copied on each plane to maintain field following meshes.
   * The first line of the file provides the total number of flux curves.
   * After that, the first column provides the normalized psi value of curve, and the second column provides desired node spacing for the corresponding flux curve.
   * An example ``meshSizeFile`` is provided below. In this example, there are 11 flux curves, and the desired node spacing on flux curves with normalized psi values of 0.2 and 0.6 are 0.005 and 0.008 respectively. 

.. code-block:: text

  11
  0.0  0.001
  0.1  0.002
  0.2  0.003
  0.3  0.004
  0.4  0.005
  0.5  0.006
  0.6  0.008
  0.7  0.010
  0.8  0.012
  0.9  0.015
  1.0  0.020

* ``meshSizeUnstructured``
   * This parameter is used to adjust the size of the elements on the model faces where unstructured triangular mesh is desired.
   * The unit used for it is centimeters. The code uses the size = 0.01*meshSizeUnstructured meters.
   * default = 1.0.

.. _sample-input:

Sample Input File
-----------------

.. code-block:: text

  !***********************************************************************************
  !******************MAGNETIC FIELD SOURCE FILES (VMEC, EQDSK for now)****************
  !***********************************************************************************

  !****************** STELLARATOR VMEC *************** 
  !The name of the vmec file for the core region
  !vmecFile wout.nc

  !****************** TOKAMAK EQDSK ***************
  !The name of eqdsk file for the tokamak cases
  eqdskFile LTX_1504291255_47400.eqdsk 

  !If set to 1, this reverses the sign of Psi in eqdsk file, 
  !allowing minimum at axis.(default 0)
  reversePsi 1

  !Factor to rescale psi (default 1.0)
  eqdPsiFactor 1.0

  !Number of 2D planes around the torus, used in spacing points on a curve.
  numPlanes 128

  !Random curve start for closed flux curve instead fo strictly in a line.
  fluxRandomStart 1

  !This is the change in phi of each rk4 step taken when moving a point along a psi curve.
  stepRadians 0.00059817477

  !All points on a psi surface are guaranteed to be within this range of the target psi value.
  psiTolerance 1E-8

  !If possible, point spacing on a curve will fall within this fraction of the desired value.
  spacingToleranceOptimal 0.6180339887498948482

  !Point spacing on a curve will always be within this fraction of the desired value.
  !The acceptable range is calculated as 1/(1+tol) to (1+tol) so a value of 1.0 gives a range 
  !of .5 to 2.0 times the desired spacing.
  spacingToleranceAbsolute 0.61803398874989484820

  !spacing between vertices option (-1: field following, -2: non-field following)
  intraCurveSpacingOption -1

  !Factors only needed when intraCurveSpacingOption is -2. If not given, default values will be used.
  intraCurveSpacingPropFacMin 0.95
  intraCurveSpacingPropFacMax 2.5

  intraCurveSpacingSmallVariation 0

  !Parameters needed and only valid for Zero X-point cases.
  lastClosedPsi 0.003100 !Required
  useWall 1 !Optional. By default wall be used. Set 0 if don't want it. 

  !***********************************************************************************
  !******************PHYSICAL COMPONENTS SOURCES (WALL CURVE FOR NOW)*****************
  !***********************************************************************************

  !limiterFile lim_adj_PT2.txt

  !***********************************************************************************
  !*******************************RESOLUTION SETTINGS*********************************
  !***********************************************************************************
  !The file containing the number and normalized psi values of flux curves
  fluxFile flux.txt 

  !The file containing the number and toroidal angles of the desired poloidal planes
  planeFile plane.txt

  !Set the mesh size from the input file
  meshSizeFile mesh_size.txt

  !Defines the mesh size at the boundary wall.(0.01*meshSizeUnstructured meters)
  meshSizeUnstructured 0.4
