# gflags (Commandline flags)
find_package(gflags REQUIRED)

# LEMON
find_package(LEMON REQUIRED)

# RapidJSON: JSON parsing
set(RAPIDJSON_USE_SSE42 ON)
find_package(RapidJSON REQUIRED)

# Include the packages dirs
include_directories(
	${LEMON_INCLUDE_DIRS}
	${RAPIDJSON_INCLUDE_DIRS}
	${GFLAGS_INCLUDE_DIRS}
)

# Now, add the flags from other libraries
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LEMON_CXX_FLAGS} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GFLAGS_CXX_FLAGS} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RAPIDJSON_CXX_FLAGS}")

add_executable(sppm
	easylogging++.cc
	geojson_reader.cc
	sppm.cc
	sppm_normal.cc
	sppm_poisson.cc
	main.cc
)

target_link_libraries(sppm
	${LEMON_LIBRARIES}
	${RAPIDJSON_LIBRARIES}
	${GFLAGS_LIBRARIES}
)

install(
	TARGETS sppm
	RUNTIME DESTINATION ${INSTALL_BIN_DIR}
	COMPONENT bin
)

