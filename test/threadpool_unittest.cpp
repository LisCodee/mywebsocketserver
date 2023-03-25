#include <gtest/gtest.h>
#include <iostream>
#include "../base/threadpool.h"
#include "../base/AsyncLog.h"

namespace
{
    class ThreadTest : public ::testing::Test
    {
    protected:
        static void SetUpTestSuite()
        {
            AsyncLog::init("");
            AsyncLog::setLevel(LOG_LEVEL_DEBUG);
        }
        static void TearDownTestSuite()
        {
            AsyncLog::uninit();
        }
        void SetUp() override
        {
            tp.reset(new ThreadPool(4, 10));
        }

        void TearDown() override
        {
            tp->stop();
        }

        std::shared_ptr<ThreadPool> tp;
    };

    void handler()
    {
        std::cout << "handler func exec" << std::endl;
    }

    // TEST_F(ThreadTest, BaseTest)
    // {
    //     tp->addTask(ThreadPool::Task(handler));
    //     tp->addTask(ThreadPool::Task(handler));
    //     tp->addTask(ThreadPool::Task(handler));
    //     tp->addTask(ThreadPool::Task(handler));
    //     tp->start();
    //     std::cout << "thread pool started" << std::endl;
    //     tp->stop();
    //     std::cout << "thread pool stop" << std::endl;
    // }
}