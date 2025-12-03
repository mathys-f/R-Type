cmake_minimum_required(VERSION 3.20)

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

add_subdirectory(src)
