CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

cmake_policy(VERSION 3.0)

PROJECT("Spatial PPM" VERSION 0.0)
SET(PROJECT_SHORT_NAME "spatial-ppm")
SET(PROJECT_DESCRIPTION_SUMMARY "Spatial PPM - Spatially Constrained Clustering via Spatial-PPM")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

IF(UNIX)
	SET(INSTALL_BIN_DIR "bin" CACHE STRING "Subdir for installing the binaries")
	SET(INSTALL_SHARE_DIR "share/${PROJECT_SHORT_NAME}" CACHE STRING "Subdir for installing shared files")
	SET(INSTALL_LICENSE_DIR "share/licenses/${PROJECT_SHORT_NAME}" CACHE STRING "Subdir for installing the licenses")
	SET(INSTALL_DOC_DIR "share/doc/${PROJECT_SHORT_NAME}" CACHE STRING "Subdir for installing the doc")
	SET(INSTALL_DATA_DIR "${INSTALL_SHARE_DIR}/data" CACHE STRING "Subdir for installing the data")
	SET(INSTALL_EXAMPLES_DIR "${INSTALL_SHARE_DIR}/examples" CACHE STRING "Subdir for installing the examples")
ELSE(UNIX)
	SET(INSTALL_BIN_DIR "." CACHE STRING "Subdir for installing the binaries")
	SET(INSTALL_SHARE_DIR "." CACHE STRING "Subdir for installing shared files")
	SET(INSTALL_LICENSE_DIR "." CACHE STRING "Subdir for installing the licenses")
	SET(INSTALL_DOC_DIR "doc" CACHE STRING "Subdir for installing the doc")
	SET(INSTALL_DATA_DIR "${INSTALL_SHARE_DIR}/data" CACHE STRING "Subdir for installing the data")
	SET(INSTALL_EXAMPLES_DIR "${INSTALL_SHARE_DIR}/examples" CACHE STRING "Subdir for installing the examples")
ENDIF(UNIX)

## Set some additional CFLAGS
# Add the needed C++11 flag
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

## These are the include directories used by the compiler.
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/include
)

ADD_SUBDIRECTORY(src)

# Install the project license
INSTALL(
	FILES LICENSE
	DESTINATION ${INSTALL_LICENSE_DIR}
	COMPONENT license
)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config/default-logger.conf ${PROJECT_BINARY_DIR}/default-logger.conf COPYONLY)
INSTALL(
	DIRECTORY config
	DESTINATION ${INSTALL_SHARE_DIR}
	COMPONENT share
)

