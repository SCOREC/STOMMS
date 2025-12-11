function (do_linking exeName)
  target_link_libraries(${exeName} PRIVATE stommsLib)
endfunction(do_linking)

# Define function for unit tests
function (unit_test exeName srcName srcMagneticField testCase)
  add_executable(${exeName} ${srcName})
  do_linking(${exeName})
  set (TEST_PATH "${TEST_DIR}/${srcMagneticField}/${testCase}")
  add_test(
    NAME "${exeName} : ${srcMagneticField}/${testCase}"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${exeName}
            ${TEST_PATH}
    WORKING_DIRECTORY ${TEST_PATH})
endfunction(unit_test)

unit_test(criticalPoints criticalPoints.cc EQDSK DIII-D_g096333.03337)
