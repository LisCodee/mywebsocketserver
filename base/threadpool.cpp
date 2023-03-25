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
    if(!bStop_)
        return;
    for (int i = 0; i < kCurrentThreadNum_; ++i)
    {
        listIdelThreads_.push_back(std::shared_ptr<Thread>(new Thread(&ThreadPool::threadFunc, this)));
    }
    bStop_ = false;
}

void net::thread::ThreadPool::stopForAllDone()
{
    bStop_ = true;
    mutex_.lock();
    while (runningNum_ != 0)
    {
        cvExit_.wait(mutex_);
    }
}

bool net::thread::ThreadPool::getTask(Task &t)
{
    mutex_.lock();
    if (queueTasks_.empty())
    {
        cvTask_.wait(mutex_);
    }
    if (bStop_)
    {
        mutex_.unlock();
        return false;
    }
    if (!queueTasks_.empty())
    {
        t = std::move(queueTasks_.front());
        queueTasks_.pop();
        return true;
    }
    mutex_.unlock();
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
        }
    }
    cvExit_.notifyAll();
    return;
}
