# -----------------------------------------------------------------------------------
#
# Finds the gflags library
#
# -----------------------------------------------------------------------------------
#
# Variables used by this module, they can change the default behaviour.
# Those variables need to be either set before calling find_package
# or exported as environment variables before running CMake:
#
# GFLAGS_INCLUDEDIR - Set custom include path, useful when the headers are
#                        outside system paths
# GFLAGS_LIBRARYDIR - Set custom include path, useful when the library is
#                        outside system paths
#
# -----------------------------------------------------------------------------------
#
# Variables defined by this module:
#
# GFLAGS_FOUND        - True if gflags was found
# GFLAGS_INCLUDE_DIRS - Path to gflags include directory
# GFLAGS_LIBRARIES    - The gflags libraries
#
# -----------------------------------------------------------------------------------
#
# Example usage:
#
#  set(GFLAGS_INCLUDEDIR "/opt/dev/gflags/include")
#
#  find_package(GFLAGS REQUIRED)
#
#  include_directories("${GFLAGS_INCLUDE_DIRS}")
#  add_executable(foo foo.cc)
#
# -----------------------------------------------------------------------------------

SET(GFLAGS_INCLUDEDIR  "" CACHE PATH "GFLAGS include directory")

foreach(opt GFLAGS_INCLUDEDIR GFLAGS_LIBRARYDIR)
	if(${opt} AND DEFINED ENV{${opt}} AND NOT ${opt} STREQUAL "$ENV{${opt}}")
		message(WARNING "Conflicting ${opt} values: ignoring environment variable and using CMake cache entry.")
	elseif(DEFINED ENV{${opt}} AND NOT ${opt})
		set(${opt} "$ENV{${opt}}")
	endif()
endforeach()

FIND_PATH(GFLAGS_INCLUDE_DIR
	gflags/gflags.h
	PATHS ${GFLAGS_INCLUDEDIR}/include ${CMAKE_SOURCE_DIR}/deps/gflags/include
	"C:/Program Files/GFLAGS/include"
)

FIND_LIBRARY(GFLAGS_LIBRARY
	NAMES gflags
	PATHS ${GFLAGS_LIBRARYDIR}/lib ${CMAKE_SOURCE_DIR}/deps/gflags/lib
	"C:/Program Files/GFLAGS/lib"
)

#####################3

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GFLAGS DEFAULT_MSG GFLAGS_LIBRARY GFLAGS_INCLUDE_DIR)

IF(GFLAGS_FOUND)
	SET(GFLAGS_INCLUDE_DIRS ${GFLAGS_INCLUDE_DIR})
	SET(GFLAGS_LIBRARIES ${GFLAGS_LIBRARY})
	SET(GFLAGS_CXX_FLAGS "-pthread")
ENDIF(GFLAGS_FOUND)

MARK_AS_ADVANCED(GFLAGS_LIBRARY GFLAGS_INCLUDE_DIR)
