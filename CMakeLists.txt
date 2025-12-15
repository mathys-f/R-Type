cmake_minimum_required(VERSION 4.2)

include(ExternalProject)

project(MyProject
    VERSION 0.1.0
    LANGUAGES CXX
)

# GLobal settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

file(
	DOWNLOAD
	https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
	${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
	EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Add dependencies below with CPMAddPackage(...)
CPMAddPackage(
	NAME raylib
	GITHUB_REPOSITORY raysan5/raylib
	GIT_TAG 5.5
)

CPMAddPackage(
	NAME glm
	GITHUB_REPOSITORY g-truc/glm
	GIT_TAG 1.0.2
)

CPMAddPackage(
    NAME lua
    GITHUB_REPOSITORY lua/lua
    GIT_TAG v5.4.4
)

ExternalProject_Add(lua_build
	PREFIX ${CMAKE_BINARY_DIR}/lua
	SOURCE_DIR ${lua_SOURCE_DIR}
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make
	INSTALL_COMMAND ""
	BUILD_IN_SOURCE 1
)

add_library(lua_lib INTERFACE)
target_include_directories(lua_lib INTERFACE ${lua_SOURCE_DIR})
add_dependencies(lua_lib lua_build)


add_compile_definitions(SOL_ALL_SAFETIES_ON)

CPMAddPackage(
	NAME sol2
	GITHUB_REPOSITORY ThePhD/sol2
	GIT_TAG v3.5.0
)

set(BOOST_ENABLE_CMAKE ON CACHE BOOL "" FORCE)
set(BOOST_INCLUDE_LIBRARIES "asio;system" CACHE STRING "" FORCE)
set(BOOST_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BOOST_BUILD_TESTS OFF CACHE BOOL "" FORCE)

CPMAddPackage(
	NAME boost
	GITHUB_REPOSITORY boostorg/boost
	GIT_TAG boost-1.85.0
)

find_package(Boost CONFIG REQUIRED COMPONENTS asio system)

add_subdirectory(src)
