# - Try to find PETSc and SuperLU libraries
# Once done this will define
#  PSPLINE_FOUND - System has PETSc
#  PSPLINE_INCLUDE_DIRS - The PSPLINE include directories
#  PSPLINE_LIBRARIES - The libraries needed to use PSPLINE
#  PSPLINE_DEFINITIONS - Compiler switches required for using PSPLINE
#
# This implementation assumes a PSPLINE install has the following structure
# VERSION/
#         include/*.h
#         lib/*.a

macro(psplineLibCheck libs isRequired)
  foreach(lib ${libs}) 
    unset(psplinelib CACHE)
    find_library(psplinelib "${lib}" PATHS ${PSPLINE_LIB_DIR})
    if(psplinelib MATCHES "^psplinelib-NOTFOUND$")
      if(${isRequired})
        message(FATAL_ERROR "PSPLINE library ${lib} not found in ${PSPLINE_LIB_DIR}")
      else()
        message("PSPLINE library ${lib} not found in ${PSPLINE_LIB_DIR}")
      endif()
    else()
      set("PSPLINE_${lib}_FOUND" TRUE CACHE INTERNAL "PSPLINE library present")
      set(PSPLINE_LIBS ${PSPLINE_LIBS} ${psplinelib})
    endif()
  endforeach()
endmacro(psplineLibCheck)

set(PSPLINE_LIBS "")
set(PSPLINE_LIB_NAMES
  ezcdf
  pspline
)

psplineLibCheck("${PSPLINE_LIB_NAMES}" TRUE)

find_path(PSPLINE_INCLUDE_DIR 
  NAMES ezcdf.mod
  PATHS ${PSPLINE_INCLUDE_DIR})
if(NOT EXISTS "${PSPLINE_INCLUDE_DIR}")
  message(FATAL_ERROR "PSPLINE include dir not found")
endif()

set(PSPLINE_LIBRARIES ${PSPLINE_LIBS} )
set(PSPLINE_INCLUDE_DIRS ${PSPLINE_INCLUDE_DIR} )

string(REGEX REPLACE 
  "/include$" "" 
  PSPLINE_INSTALL_DIR
  "${PSPLINE_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PARMETIS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PSPLINE  DEFAULT_MSG
                                  PSPLINE_LIBS PSPLINE_INCLUDE_DIR)

mark_as_advanced(PSPLINE_INCLUDE_DIR PSPLINE_LIBS)

set(PSPLINE_LINK_LIBS "")
foreach(lib ${PSPLINE_LIB_NAMES})
  set(PSPLINE_LINK_LIBS "${PSPLINE_LINK_LIBS} -l${lib}")
endforeach()

#pkgconfig  
set(prefix "${PSPLINE_INSTALL_DIR}")
set(includedir "${PSPLINE_INCLUDE_DIR}")
configure_file(
  "${CMAKE_HOME_DIRECTORY}/cmake/libPspline.pc.in"
  "${CMAKE_BINARY_DIR}/libPspline.pc"
  @ONLY)

INSTALL(FILES "${CMAKE_BINARY_DIR}/libPspline.pc" DESTINATION lib/pkgconfig)

