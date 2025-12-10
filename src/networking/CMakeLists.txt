file(GLOB_RECURSE NETWORKING_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT NETWORKING_SOURCES)
    message(FATAL_ERROR "No SOURCES given to target: networking (no .cpp files found in ${CMAKE_CURRENT_SOURCE_DIR})")
endif()

add_library(networking ${NETWORKING_SOURCES})

target_include_directories(networking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Threads REQUIRED)

target_link_libraries(networking
    PUBLIC
        Boost::asio
        Boost::system
        Threads::Threads
)
