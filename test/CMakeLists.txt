project(Google_tests)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FETCHCONTENT_MAKEAVAILABLE(googletest)
include(GoogleTest)
enable_testing()

add_executable(
        unittests
        token_test.cpp
        main.cpp)
target_link_libraries(unittests GTest::gtest_main gtest gtest_main monke_core)
gtest_discover_tests(unittests)
