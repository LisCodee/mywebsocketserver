#include "threadpool.h"
#include "AsyncLog.h"
#include <iostream>

int net::thread::ThreadPool::kPoolNums = 0;

net::thread::ThreadPool::ThreadPool(int numThreads, int maxThreads)
    : kCurrentThreadNum_(numThreads), kMaxThreadNum_(maxThreads),
      kPoolIdx_(ThreadPool::kPoolNums++), bStop_(true), runningNum_(0)
{
}

net::thread::ThreadPool::~ThreadPool()
{
}

void net::thread::ThreadPool::start()
{
    if (!bStop_)
        return;
    for (int i = 0; i < kCurrentThreadNum_; ++i)
    {
        auto f = std::bind(&ThreadPool::threadLoop, this);
        std::shared_ptr<std::thread> spThread(new std::thread(f, this));
        listIdelThreads_.push_back(spThread);
    }
    bStop_ = false;
}

bool net::thread::ThreadPool::stopForAllDone()
{
    bStop_ = true;
    std::unique_lock<std::mutex> lock(mutex_);
    while (runningNum_ != 0)
    {
        cvExit_.wait(lock);
    }
    return true;
}

bool net::thread::ThreadPool::getTask(Task &t)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (queueTasks_.empty())
    {
        cvTask_.wait(lock);
    }
    if (bStop_)
    {
        return false;
    }
    if (!queueTasks_.empty())
    {
        t = std::move(queueTasks_.front());
        queueTasks_.pop();
        return true;
    }
    return false;
}

void net::thread::ThreadPool::threadLoop()
{
    while (!bStop_)
    {
        Task task;
        if (getTask(task))
        {
            ++runningNum_;
            task();
            --runningNum_;
            int a = runningNum_;
            LOGI("this is %d tasks running in pool:%d", a, kPoolIdx_);
        }
    }
    while (runningNum_)
        std::this_thread::yield();
    cvExit_.notify_all();
    return;
}
