#include <gtest/gtest.h>
#include <iostream>
#ifndef WIN32
#include "../linuxCode/Thread.h"

using net::thread::Thread;

void* testFunc(void* val)
{
    int* retVal = new int;
    *retVal = *(int*)val;
    std::cout << *retVal << std::endl;
    // usleep(1000);
    return (void*)retVal;
}

TEST(ThreadTest, ThreadBaseTest)
{
    int* arg = new int;
    *arg = 100;
    Thread t(testFunc, (void*) arg, false);
    EXPECT_EQ(t.joinable(), true);
    EXPECT_EQ(t == t, true);
    EXPECT_GT(t.getId(), 0);
    int* res = (int*)t.join();
    EXPECT_EQ(*res, *arg);
}

TEST(ThreadTest, ThreadDetachTest)
{
    int* arg = new int(100);
    Thread t(testFunc, (void*) arg, true);
    EXPECT_EQ(t.joinable(), false);
    EXPECT_EQ(t == t, true);
    EXPECT_GT(t.getId(), 0);
    EXPECT_EQ(t.join(), nullptr);
}

TEST(ThreadTest, ThreadCancelTest)
{
    int* arg = new int(100);
    Thread t(testFunc, (void*) arg, true);
    std::cout << "cancel number:" << t.cancel() << std::endl;
    EXPECT_EQ(t.joinable(), false);
}
#endif