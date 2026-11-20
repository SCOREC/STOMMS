# STOMMS testing for TOMMS Cases

In STOMMS few of the TOMMS parameters are made obselete because those parameters were not needed 
given the new developments. Also, there are few parameters that are fixed in STOMMS and they don't
need a value from users. Some of such parameters were still used in the testing for TOMMS. This 
resulted in failing of the STOMMS tests when I tested TOMMS sample cases (with TOMMS input files). 
To verify and update these test cases for STOMMS, I re ran TOMMS without the obselete(for STOMMS) 
parameters and re adjusted the tests accordingly. I am documenting what parameters were removed and
how they affected the tests.

* **`inter_curve_spacing_min`**
  - This parameters deletes a flux curve if it's radial spacing from axis is less the ``inter_curve_spacing_min``.
  - This caused a mismatch in number of model entities (vertices, edges, faces) by 1 on running same case in STOMMS.
  - This parameter was helpful when taking in absolute value of spacing from users, instead of a ordered list of psi values of flux curves.
  - The test cases where this caused an issue are:
     - LTX\_1504291255\_47400
     - ltx\_1504291255\_47400\_xgca

* **`parallel_cutoff`** 
  - A factor to decide if given three points are on a straight line or not.
  - If 1 - ``parallel_cutoff`` < sin( 3 points ) : considered as a geometric corner.
  - Otherwise, 3 points are considered on a straight line.
  - Instead of taking this value from users, STOMMS uses a default factor of 0.99999.
  - This caused a mismatch in the number of model vertices and edges on wall for almost every test case.

* **`inter_curve_spacing_axis_forcing`**
  - This parameter is used to set a distance between the axis and its adjacent radial node (i.e. the first next point from the axis) on outboard midplane.
  - STOMMS doesn't require this parameter.
  - Although it didn't affect any of the given test cases, I disabled it in TOMMS testing to make sure no mismatches are caused by this parameter.

* **`validOpenCurve`**
  - It's not a parameter. It a function in the code to check the if an open curve should be considered for further processing or not.
  - It basically checks if the an open curve is parallel to the wall curve boundary and overlapping it within a tolerance. 
  - The test case``ITER_10MA_xgca`` showed mismatch because of this. 
  - The mismatch was in model vertices (+3 in STOMMS), model edges (+1 in STOMMS), and model faces (+1 in STOMMS).
  - This ``validOpenCurve`` has been added to the STOMMS and now entities count is consistent.

* **`Meshing Issue`**
  - There is a block of code in TOMMS that sets mesh size on the model edges that are classified on the flux curves.
  - Since mesh vertices and edges are specified on the flux curves, this shouldn't be used or affect mesh size on flux curve.
  - It didn't cause any mismatch on the size of mesh on the flux curves, or one element mesh faces but if a flux curve was adjacent to a model face with unstrucutred mesh, this setting affected gradation on unstructured face. I disabled this block of code in TOMMS before testing to make sure things are consistent.
  - The block of code is:
    * https://github.com/SCOREC/tomms/blob/8a6b1b7ed4ab2807f7e61e26dba80a98a368f4a3/xgc/meshing.cc#L243-L263

* **`fluxCurvesInPrivate`**
  - Adding flux curves in the private region is optional in TOMMS. In STOMMS, it was hardcoded to true for every case.
  - This caused a mismatch for the test cases that were not adding flux curves in private region in TOMMS testing.
  - This has been added as a parameter and it now uses a default value of false just as in TOMMS.

* **`Mesh Size Interpolation on Flux Curves`**
  - In STOMMS, users provide a list of flux curves (with psi values) in a file, and another file for mesh size on each flux curves. The second file has two columns, one for the psi values and second for mesh sizes on the flux curves defined by those psi values.
  - STOMMS assumed that list of psi values are consistent in both the files. But this is not the case in TOMMS. 
  - In TOMMS, psi values could be different in both files, and also, mesh size file can have fewer values than the file with list of flux curves.
  - Mesh size on any flux curve (psi value) is evaluated using the interpolation of the data in the mesh size file.
  - Since STOMMS assumed direct mapping of both the files, this lead to interpolation mismatches. This has been fixed in STOMMS now.
