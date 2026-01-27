cmake_minimum_required(VERSION 3.20)

project(R-Type
    VERSION 0.1.0
    LANGUAGES CXX C
)

find_program(CONAN_CMD conan)
option(USE_CONAN "Force Use Conan" OFF)

if(NOT USE_CONAN AND CONAN_CMD)
    message(STATUS "Checking network connectivity to GitHub for CPM...")
    file(DOWNLOAD "https://github.com" ${CMAKE_BINARY_DIR}/github_ping
        TIMEOUT 5
        STATUS PING_STATUS
        LOG PING_LOG
    )
    list(GET PING_STATUS 0 PING_CODE)

    if(PING_CODE EQUAL 0)
        message(STATUS "Network OK. Using CPM (Primary).")
    else()
        message(WARNING "Network unreachable (Error: ${PING_CODE}). Switching to Conan fallback (Secondary).")
        set(USE_CONAN ON)
    endif()
endif()
# -------------------------------

if(USE_CONAN)
    if(NOT CONAN_CMD)
        message(WARNING "Conan requested (or triggered by fallback) but 'conan' not found.")
    else()
        message(STATUS "Conan active: ${CONAN_CMD}. Preparing local dependencies...")

        function(run_conan_install BUILD_TYPE)
            message(STATUS "Running Conan for build type: ${BUILD_TYPE}")
            execute_process(
                COMMAND ${CONAN_CMD} install ${CMAKE_SOURCE_DIR}
                        --output-folder=${CMAKE_BINARY_DIR}
                        --build=missing
                        -s build_type=${BUILD_TYPE}
                RESULT_VARIABLE CONAN_RESULT
            )
            if(NOT CONAN_RESULT EQUAL 0)
                message(WARNING "Conan install failed for ${BUILD_TYPE}.")
            else()
                set(CPM_USE_LOCAL_PACKAGES ON PARENT_SCOPE)

                file(GLOB_RECURSE CONAN_TOOLCHAINS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")

                foreach(TOOLCHAIN_FILE ${CONAN_TOOLCHAINS})
                    get_filename_component(GENERATORS_DIR ${TOOLCHAIN_FILE} DIRECTORY)
                    list(APPEND CMAKE_PREFIX_PATH "${GENERATORS_DIR}")
                    list(APPEND CMAKE_MODULE_PATH "${GENERATORS_DIR}")
                endforeach()

                set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
                set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
            endif()
        endfunction()

        get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if(IS_MULTI_CONFIG)
            foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
                if(NOT TYPE STREQUAL "MinSizeRel" AND NOT TYPE STREQUAL "RelWithDebInfo")
                    run_conan_install(${TYPE})
                endif()
            endforeach()
        else()
            if(NOT CMAKE_BUILD_TYPE)
                set(CMAKE_BUILD_TYPE Release)
            endif()
            run_conan_install(${CMAKE_BUILD_TYPE})
        endif()

        list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
    endif()
endif()

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# CPM source cache for CI/CD optimization
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cache/CPM" CACHE PATH "CPM source cache")

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
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_GAMES OFF"
)

if(TARGET raylib::raylib AND NOT TARGET raylib)
    add_library(raylib ALIAS raylib::raylib)
endif()

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.2
    OPTIONS
        "GLM_BUILD_TESTS OFF"
)

if(TARGET glm::glm AND NOT TARGET glm)
    add_library(glm ALIAS glm::glm)
endif()

if(CPM_USE_LOCAL_PACKAGES)
    find_package(lua QUIET)
endif()

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
elseif(TARGET lua::lua AND NOT TARGET lua)
    add_library(lua ALIAS lua::lua)
endif()

add_compile_definitions(SOL_ALL_SAFETIES_ON)

CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    GIT_TAG v3.5.0
)

if(TARGET sol2::sol2 AND NOT TARGET sol2)
    add_library(sol2 ALIAS sol2::sol2)
endif()

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

    # Windows-specific settings
    if(WIN32)
        target_compile_definitions(asio INTERFACE 
            _WIN32_WINNT=0x0601  # Windows 7 or later
            NOGDI                 # Prevents Windows GDI conflicts with raylib
            NOUSER                # Prevents Windows USER conflicts with raylib
        )
        target_link_libraries(asio INTERFACE ws2_32)
    endif()
    
    find_package(Threads REQUIRED)
    target_link_libraries(asio INTERFACE Threads::Threads)
elseif(TARGET asio::asio AND NOT TARGET asio)
    add_library(asio ALIAS asio::asio)
endif()

# cpp-httplib for HTTP client requests to Node backend
if(CPM_USE_LOCAL_PACKAGES)
    find_package(httplib QUIET)
    if(httplib_FOUND AND NOT TARGET cpp-httplib::cpp-httplib)
        add_library(cpp-httplib::cpp-httplib ALIAS httplib::httplib)
    endif()
endif()

CPMAddPackage(
    NAME cpp-httplib
    GITHUB_REPOSITORY yhirose/cpp-httplib
    GIT_TAG v0.15.3
    DOWNLOAD_ONLY YES
)

if(cpp-httplib_ADDED)
    set(CPP_HTTPLIB_INCLUDE_DIR ${cpp-httplib_SOURCE_DIR})
elseif(TARGET cpp-httplib::cpp-httplib)
    get_target_property(CPP_HTTPLIB_INCLUDE_DIR cpp-httplib::cpp-httplib INTERFACE_INCLUDE_DIRECTORIES)
endif()

# nlohmann/json for JSON serialization
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    GIT_TAG v3.11.3
)

CPMAddPackage(
    NAME Boost
    VERSION 1.84.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
    OPTIONS
        "BOOST_ENABLE_CMAKE ON"
        "BOOST_INCLUDE_LIBRARIES interprocess;system;filesystem;date_time"
)

# Fix for Conan Boost targets
if(TARGET Boost::boost AND NOT TARGET Boost::interprocess)
    add_library(Boost::interprocess ALIAS Boost::boost)
endif()

# Only for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: enabling automatic analysis")
        set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
    else()
        message(STATUS "clang-tidy not found: skipping automatic analysis")
    endif()
endif()

add_subdirectory(src)
