# -----------------------------------------------------------------------------------
#
# Finds the RapidJSON library
#
# -----------------------------------------------------------------------------------
#
# Variables used by this module, they can change the default behaviour.
# Those variables need to be either set before calling find_package
# or exported as environment variables before running CMake:
#
# RAPIDJSON_INCLUDEDIR - Set custom include path, useful when RapidJSON headers are
#                        outside system paths
# RAPIDJSON_USE_SSE2   - Configure RapidJSON to take advantage of SSE2 capabilities
# RAPIDJSON_USE_SSE42  - Configure RapidJSON to take advantage of SSE4.2 capabilities
#
# -----------------------------------------------------------------------------------
#
# Variables defined by this module:
#
# RAPIDJSON_FOUND        - True if RapidJSON was found
# RAPIDJSON_INCLUDE_DIRS - Path to RapidJSON include directory
# RAPIDJSON_CXX_FLAGS    - Extra C++ flags required for compilation with RapidJSON
#
# -----------------------------------------------------------------------------------
#
# Example usage:
#
#  set(RAPIDJSON_USE_SSE2 ON)
#  set(RAPIDJSON_INCLUDEDIR "/opt/github.com/rjeczalik/rapidjson/include")
#
#  find_package(RapidJSON REQUIRED)
#
#  include_directories("${RAPIDJSON_INCLUDE_DIRS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RAPIDJSON_CXX_FLAGS}")
#  add_executable(foo foo.cc)
#
# -----------------------------------------------------------------------------------

SET(RAPIDJSON_INCLUDEDIR  "" CACHE PATH "RapidJSON root directory")

foreach(opt RAPIDJSON_INCLUDEDIR RAPIDJSON_USE_SSE2 RAPIDJSON_USE_SSE42)
	if(${opt} AND DEFINED ENV{${opt}} AND NOT ${opt} STREQUAL "$ENV{${opt}}")
		message(WARNING "Conflicting ${opt} values: ignoring environment variable and using CMake cache entry.")
	elseif(DEFINED ENV{${opt}} AND NOT ${opt})
		set(${opt} "$ENV{${opt}}")
	endif()
endforeach()

find_path(
	RAPIDJSON_INCLUDE_DIR
	NAMES rapidjson/rapidjson.h
	PATHS ${RAPIDJSON_ROOT_DIR} ${CMAKE_SOURCE_DIR}/deps/rapidjson/include
	DOC "Include directory for the RapidJSON library."
)

set(RAPIDJSON_LIBRARY "")

if(RAPIDJSON_USE_SSE42)
	set(RAPIDJSON_CXX_FLAGS "-DRAPIDJSON_SSE42")
	if(MSVC)
		set(RAPIDJSON_CXX_FLAGS "${RAPIDJSON_CXX_FLAGS} /arch:SSE4.2")
	else()
		set(RAPIDJSON_CXX_FLAGS "${RAPIDJSON_CXX_FLAGS} -msse4.2")
	endif()
else()
	if(RAPIDJSON_USE_SSE2)
		set(RAPIDJSON_CXX_FLAGS "-DRAPIDJSON_SSE2")
		if(MSVC)
			set(RAPIDJSON_CXX_FLAGS "${RAPIDJSON_CXX_FLAGS} /arch:SSE2")
		else()
			set(RAPIDJSON_CXX_FLAGS "${RAPIDJSON_CXX_FLAGS} -msse2")
		endif()
	endif()
endif()

mark_as_advanced(RAPIDJSON_CXX_FLAGS)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RAPIDJSON DEFAULT_MSG RAPIDJSON_INCLUDE_DIR)

IF(RAPIDJSON_FOUND)
	SET(RAPIDJSON_INCLUDE_DIRS ${RAPIDJSON_INCLUDE_DIR})
	SET(RAPIDJSON_LIBRARIES ${RAPIDJSON_LIBRARY})
ENDIF(RAPIDJSON_FOUND)

MARK_AS_ADVANCED(RAPIDJSON_LIBRARY RAPIDJSON_INCLUDE_DIR)
