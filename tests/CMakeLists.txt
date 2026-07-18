add_executable(
    HexiumTests
    test_main.cpp
    math_custom_tests/Vector3/Vector3.Constructors.cpp
)

target_link_libraries(
    HexiumTests
    PRIVATE
    GTest::gtest_main
    HexiumLib
)

include(GoogleTest)

gtest_discover_tests(HexiumTests)