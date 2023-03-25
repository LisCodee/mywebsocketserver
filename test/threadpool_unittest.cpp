#include <gtest/gtest.h>
#include <iostream>
#include "../base/threadpool.h"
#include "../base/AsyncLog.h"

namespace
{
    using net::thread::ThreadPool;
    class ThreadPoolTest : public ::testing::Test
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
            tp.reset(new ThreadPool(4, 4));
        }

        void TearDown() override
        {
            
        }

        std::shared_ptr<ThreadPool> tp;
    };

    void handler()
    {
        std::cout << "handler func exec" << std::endl;
    }

    int intHandler(int a)
    {
        std::cout << "input a:" << a << std::endl;
        return a;
    }

    TEST_F(ThreadPoolTest, BaseTest)
    {
        tp->exec(ThreadPool::Task(handler));
        tp->exec(ThreadPool::Task(handler));
        tp->exec(ThreadPool::Task(handler));
        tp->exec(std::function<int(int)>(intHandler), 100);
        auto ret = tp->exec(ThreadPool::Task(handler));
        tp->start();
        std::cout << "thread pool started" << std::endl;
        // tp->stopForAllDone();
        std::cout << "thread pool stop" << std::endl;
    }
}