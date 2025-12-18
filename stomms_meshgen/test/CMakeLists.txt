function (do_linking exeName)
  target_link_libraries(${exeName} PRIVATE stommsLib)
endfunction(do_linking)

# Define function for unit tests
function (unit_test exeName srcMagneticField testCase)
  set (TEST_PATH "${TEST_DIR}/${srcMagneticField}/${testCase}")
  add_test(
    NAME "${exeName}:${srcMagneticField}/${testCase}"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${exeName}
            ${testCase}
    WORKING_DIRECTORY ${TEST_PATH})
endfunction(unit_test)

# Critical Points Search Method
set(exeName criticalPoints)
add_executable(${exeName} criticalPoints.cc)
do_linking(${exeName})
unit_test(${exeName} EQDSK DIIID-g096333.03337)
unit_test(${exeName} EQDSK KSTAR-g018451.002790_kin_1)
unit_test(${exeName} EQDSK NSTX-g132588.00650)
unit_test(${exeName} EQDSK LTX-1504291255_47400.eqdsk)
unit_test(${exeName} EQDSK MASTU-g_p45152_t0.80000)
