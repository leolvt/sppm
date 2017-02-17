# -----------------------------------------------------------------------------------
#
# Finds the LEMON library
#
# -----------------------------------------------------------------------------------
#
# Variables used by this module, they can change the default behaviour.
# Those variables need to be either set before calling find_package
# or exported as environment variables before running CMake:
#
# LEMON_INCLUDEDIR - Set custom include path, useful when the headers are
#                        outside system paths
# LEMON_LIBRARYDIR - Set custom include path, useful when the library is
#                        outside system paths
#
# -----------------------------------------------------------------------------------
#
# Variables defined by this module:
#
# LEMON_FOUND        - True if LEMON was found
# LEMON_INCLUDE_DIRS - Path to LEMON include directory
# LEMON_LIBRARIES    - The LEMON libraries
#
# -----------------------------------------------------------------------------------
#
# Example usage:
#
#  set(LEMON_INCLUDEDIR "/opt/dev/lemon/include")
#
#  find_package(LEMON REQUIRED)
#
#  include_directories("${LEMON_INCLUDE_DIRS}")
#  add_executable(foo foo.cc)
#
# -----------------------------------------------------------------------------------

SET(LEMON_INCLUDEDIR  "" CACHE PATH "LEMON include directory")

foreach(opt LEMON_INCLUDEDIR LEMON_LIBRARYDIR)
	if(${opt} AND DEFINED ENV{${opt}} AND NOT ${opt} STREQUAL "$ENV{${opt}}")
		message(WARNING "Conflicting ${opt} values: ignoring environment variable and using CMake cache entry.")
	elseif(DEFINED ENV{${opt}} AND NOT ${opt})
		set(${opt} "$ENV{${opt}}")
	endif()
endforeach()

FIND_PATH(LEMON_INCLUDE_DIR
	lemon/core.h
	PATHS ${LEMON_INCLUDEDIR}/include ${CMAKE_SOURCE_DIR}/deps/lemon/include
	"C:/Program Files/LEMON/include"
)

FIND_LIBRARY(LEMON_LIBRARY
	NAMES lemon emon
	PATHS ${LEMON_LIBRARYDIR}/lib ${CMAKE_SOURCE_DIR}/deps/lemon/lib
	"C:/Program Files/LEMON/lib"
)

#####################3

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LEMON DEFAULT_MSG LEMON_LIBRARY LEMON_INCLUDE_DIR)

IF(LEMON_FOUND)
	SET(LEMON_INCLUDE_DIRS ${LEMON_INCLUDE_DIR})
	SET(LEMON_LIBRARIES ${LEMON_LIBRARY})
ENDIF(LEMON_FOUND)

MARK_AS_ADVANCED(LEMON_LIBRARY LEMON_INCLUDE_DIR)
