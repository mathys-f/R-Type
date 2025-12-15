cmake_minimum_required(VERSION 3.20)

include(ExternalProject)

project(MyProject
    VERSION 0.1.0
    LANGUAGES CXX C
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
    GIT_TAG v5.4.6
    DOWNLOAD_ONLY YES
)

if(lua_ADDED)
    # Create our own CMake target for Lua
    add_library(lua STATIC
        ${lua_SOURCE_DIR}/lapi.c
        ${lua_SOURCE_DIR}/lcode.c
        ${lua_SOURCE_DIR}/lctype.c
        ${lua_SOURCE_DIR}/ldebug.c
        ${lua_SOURCE_DIR}/ldo.c
        ${lua_SOURCE_DIR}/ldump.c
        ${lua_SOURCE_DIR}/lfunc.c
        ${lua_SOURCE_DIR}/lgc.c
        ${lua_SOURCE_DIR}/llex.c
        ${lua_SOURCE_DIR}/lmem.c
        ${lua_SOURCE_DIR}/lobject.c
        ${lua_SOURCE_DIR}/lopcodes.c
        ${lua_SOURCE_DIR}/lparser.c
        ${lua_SOURCE_DIR}/lstate.c
        ${lua_SOURCE_DIR}/lstring.c
        ${lua_SOURCE_DIR}/ltable.c
        ${lua_SOURCE_DIR}/ltm.c
        ${lua_SOURCE_DIR}/lundump.c
        ${lua_SOURCE_DIR}/lvm.c
        ${lua_SOURCE_DIR}/lzio.c
        ${lua_SOURCE_DIR}/lauxlib.c
        ${lua_SOURCE_DIR}/lbaselib.c
        ${lua_SOURCE_DIR}/lcorolib.c
        ${lua_SOURCE_DIR}/ldblib.c
        ${lua_SOURCE_DIR}/liolib.c
        ${lua_SOURCE_DIR}/lmathlib.c
        ${lua_SOURCE_DIR}/loadlib.c
        ${lua_SOURCE_DIR}/loslib.c
        ${lua_SOURCE_DIR}/lstrlib.c
        ${lua_SOURCE_DIR}/ltablib.c
        ${lua_SOURCE_DIR}/lutf8lib.c
        ${lua_SOURCE_DIR}/linit.c
    )

    target_include_directories(lua PUBLIC ${lua_SOURCE_DIR})

    if(UNIX AND NOT APPLE)
        target_link_libraries(lua m dl)
    endif()
endif()

add_compile_definitions(SOL_ALL_SAFETIES_ON)

CPMAddPackage(
	NAME sol2
	GITHUB_REPOSITORY ThePhD/sol2
	GIT_TAG v3.5.0
)

# Use standalone Asio instead of full Boost
CPMAddPackage(
	NAME asio
	GITHUB_REPOSITORY chriskohlhoff/asio
	GIT_TAG asio-1-30-2
	DOWNLOAD_ONLY YES
)

if(asio_ADDED)
	add_library(asio INTERFACE)
	target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
	target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
	find_package(Threads REQUIRED)
	target_link_libraries(asio INTERFACE Threads::Threads)
endif()

add_subdirectory(src)
