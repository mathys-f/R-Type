add_executable(networking_tests)

file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_sources(networking_tests PRIVATE ${TEST_SOURCES})

target_link_libraries(networking_tests
    PRIVATE
        networking
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(networking_tests)
