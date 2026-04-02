function (do_linking exeName)
  target_link_libraries(${exeName} PRIVATE stommsLib)
endfunction(do_linking)

# Define function for unit tests
function (reg_test exeName srcMagneticField testCase)
  set (TEST_PATH "${TEST_DIR}/${srcMagneticField}/${testCase}")
  add_test(
    NAME "${exeName}:${srcMagneticField}/${testCase}"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${exeName}
            ${CMAKE_BINARY_DIR}/stomms_meshgen/stomms_meshgen
            ${TEST_PATH}
    WORKING_DIRECTORY ${TEST_PATH})
endfunction(reg_test)

# Critical Points Search Method
set(exeName regressionTest)
add_executable(${exeName} regTest.cc)
do_linking(${exeName})

# Tokamak Cases
reg_test(${exeName} EQDSK DIIID-g096333.03337)
#reg_test(${exeName} EQDSK ITER-10MA.geqdsk-xgca)
#reg_test(${exeName} EQDSK KSTAR-g018451.002790_kin_1)
reg_test(${exeName} EQDSK MASTU-g_p45152_t0.80000)
#reg_test(${exeName} EQDSK NSTX-g132588.00650)

# Stellarator Cases
reg_test(${exeName} VMEC HSX)
reg_test(${exeName} VMEC NCSX-wout_ncsx_c09r00_free.nc)
reg_test(${exeName} VMEC W7-X)

