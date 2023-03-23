cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(myserver VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -O0 -Wno-unused-variable -pthread")

set(unit_test
    test/asynclog_unittest.cpp
    test/socket_unittest.cpp
    test/time_unittest.cpp
    test/_main.cpp
    )
set(utils 
    base/AsyncLog.h
    base/AsyncLog.cpp
    base/time.h
    base/time.cpp
    )
set(net 
    net/net.h
    net/net.cpp)
set(base )
# set(src src/main.cpp)

enable_testing()

add_executable(myserver ${utils} ${unit_test} ${src} ${net})
target_link_libraries(myserver PUBLIC 
    /usr/local/lib/libgtest.a
    /usr/local/lib/libgtest_main.a
)