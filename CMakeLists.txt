cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(myserver VERSION 1.0)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/3rdparty)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


set(test 
test/asynclog_unittest.cpp)
set(utils 
base/AsyncLog.h
base/AsyncLog.cpp
)
set(net )
set(base )
set(src src/main.cpp)


enable_testing()

add_executable(myserver ${test} ${utils} ${src})

target_link_libraries(
    myserver 
    GoogleTest
)

include(GoogleTest)
gtest_discover_tests(myserver)