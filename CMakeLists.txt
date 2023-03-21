cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(myserver VERSION 1.0)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

set(test 
test/hello_test.cpp)
set(utils )
set(net )
set(base )


enable_testing()

add_executable(server ${test})

target_link_libraries(
        server
        GTest::gtest_main
        )

include(GoogleTest)
gtest_discover_tests(server)