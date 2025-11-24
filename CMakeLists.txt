cmake_minimum_required(VERSION 3.24)
project(R-Type VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(
	DOWNLOAD
	https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
	${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
	EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Add dependencies below with CPMAddPackage(...)

file(GLOB_RECURSE RTYPE_CLIENT_SOURCES client/*.cpp client/**/*.cpp)
file(GLOB_RECURSE RTYPE_SERVER_SOURCES server/*.cpp server/**/*.cpp)

add_executable(R-Type ${RTYPE_CLIENT_SOURCES})
add_executable(R-Type-server ${RTYPE_SERVER_SOURCES})

target_compile_features(R-Type PRIVATE cxx_std_23)
target_compile_features(R-Type-server PRIVATE cxx_std_23)
