# TOMMS input compatibility tests (xgc_mesh_in).
# Uses the TOMMS samples kept outside of this repository (TOMMS_TEST_DIR,
# set in the top level CMakeLists.txt).

# Get the executable for test script
add_executable(test_exe test.cc)

# Link required libraries
target_link_libraries(test_exe PRIVATE stommsLib)

function(reg_test TESTNAME)
  add_test(
    NAME "TOMMS_INPUT_COMPATIBILITY_TEST/${TESTNAME}"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_exe
            ${CMAKE_BINARY_DIR}/stomms_meshgen/stomms_meshgen
            ${TEST_DIR}/tommsInputFiles/${TESTNAME}
    WORKING_DIRECTORY ${TEST_DIR}/tommsInputFiles/${TESTNAME})
endfunction(reg_test)

# Add all the test cases here
# Only cases with radial_uniform_meshing_unit = -1 and intra_curve_spacing_option = -1 or -2
reg_test(coarse)

reg_test(d161218n_LH_D)
reg_test(d3d_144981-03175_meshing_v2)
reg_test(d3d_163240)
reg_test(D-g157308.04200)

reg_test(ITER_10MA_xgca)

reg_test(kstar_018451_2790_xgc1)
reg_test(kstar_018451_2790_xgca)

reg_test(LTX_1504291255_47400)
reg_test(ltx_1504291255_47400_xgc1)
reg_test(ltx_1504291255_47400_xgca)

reg_test(nstx_132588_00650_v2)
reg_test(nstx_132588_development_sample)
reg_test(nstx_132588_em_20210726)
reg_test(nstx_132588_v3)
reg_test(nstx_132588_v4)

#***************************************************************
# Large production scale cases that takes more than 15 minutes.
# Not suitable for quick testing.
# Can be added if time and memory constraints are not there.

#reg_test(ITER_10MA)
#reg_test(ITER_15MA_H_EM_20210922)
#***************************************************************
