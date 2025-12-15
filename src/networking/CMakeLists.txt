add_library(networking)

file(GLOB_RECURSE NETWORKING_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_sources(networking PRIVATE ${NETWORKING_SOURCES})

target_include_directories(networking PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Threads REQUIRED)

target_link_libraries(networking
    PUBLIC
        asio
        Threads::Threads
)
