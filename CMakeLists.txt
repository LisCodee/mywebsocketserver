cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(myserver VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -O0 -Wno-unused-variable -pthread")

set(linux_src 
    linuxCode/Thread.h
    linuxCode/Thread.cpp
    linuxCode/sync.h
    linuxCode/sync.cpp
    )

set(unit_test
    test/asynclog_unittest.cpp
    test/socket_unittest.cpp
    test/time_unittest.cpp
    test/threadpool_unittest.cpp
    test/_main.cpp
    test/thread_unittest.cpp
    )
set(utils 
    base/AsyncLog.h
    base/AsyncLog.cpp
    base/time.h
    base/time.cpp
    base/threadpool.h
    base/threadpool.cpp
    )
set(net 
    net/net.h
    net/net.cpp)
set(src src/main.cpp)

enable_testing()

add_executable(unittest ${utils} ${unit_test} ${net} ${linux_src})
add_executable(myserver ${utils}  ${src} ${net} ${linux_src})
target_link_libraries(unittest PUBLIC 
    /usr/local/lib/libgtest.a
    /usr/local/lib/libgtest_main.a
)
