add_executable(
    ShanTT_Tests
    test_main.cpp
)

add_subdirectory(
    math_custom_tests
)


target_link_libraries(
    ShanTT_Tests
    PRIVATE
    GTest::gtest_main
    ShanTT_Lib
)

include(GoogleTest)

gtest_discover_tests(ShanTT_Tests)