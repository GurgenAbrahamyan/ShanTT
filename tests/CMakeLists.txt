add_executable(
    HexiumTests
    test_main.cpp
)

target_link_libraries(
    HexiumTests
    PRIVATE
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(HexiumTests)