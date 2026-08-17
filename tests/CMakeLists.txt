add_executable(
    ShanTT_Tests
    test_main.cpp
)

add_subdirectory(
    math_custom_tests
)

add_subdirectory(
    scene_tests
)

target_link_libraries(
    ShanTT_Tests
    PRIVATE
        GTest::gtest_main
        ShanTT::Engine
)

include(GoogleTest)

gtest_discover_tests(ShanTT_Tests)