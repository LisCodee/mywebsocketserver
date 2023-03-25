#include "threadpool.h"
#include "AsyncLog.h"
#include <iostream>

int ThreadPool::kPoolNums = 0;

ThreadPool::ThreadPool(int numThreads, int maxThreads)
    : bStart_(false), bStop_(false),
      kCurrentThreadNum_(0), kMaxThreadNum_(maxThreads)
{
    kPoolIdx_ = kPoolNums++;
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start()
{
    if (bStart_)
        return;
    bStart_ = true;
    for (int i = 0; i < kMaxThreadNum_; ++i)
    {
        listThreads_.push_back(
            std::shared_ptr<std::thread>(new std::thread(std::bind(&ThreadPool::threadLoop, this))));
    }
    LOGD("Thread pool %d started, create all threads", kPoolIdx_);
}

void ThreadPool::stop()
{
    if (bStop_)
        return;
    // 唤醒所有线程
    std::unique_lock<std::mutex> lock(mutexTask_);
    bStart_ = false;
    bStop_ = true;
    cvTask_.notify_all();
    LOGD("Thread pool %d stopping, notify all threads", kPoolIdx_);

    for (auto &t : listThreads_)
    {
        if (t->joinable())
        {
            t->join();
        }
        else
        {
            LOGW("can't join the thread in pool:%d", kPoolIdx_);
        }
    }
    // 清空线程池
    listThreads_.clear();
}

void ThreadPool::addTask(const Task& t)
{
    std::unique_lock<std::mutex> lock(mutexTask_);
    queueTasks_.push(t);
}



void ThreadPool::threadLoop()
{
    LOGD("thread in pool:%d start run.", kPoolIdx_);
    while(bStart_)
    {
        std::cout << "thread getTask()" << std::endl;
        ThreadPool::Task t = getTask();
        if(t)
        {
            t();
        }
    }
}

ThreadPool::Task ThreadPool::getTask()
{
    Task t;

    std::unique_lock<std::mutex> lock(mutexTask_);
    // 等待任务执行
    while (queueTasks_.empty() && bStart_)
    {
        cvTask_.wait(lock);
    }
    t = queueTasks_.front();
    queueTasks_.pop();
    LOGD("thread pool:%d thread: get a task.", kPoolIdx_);

    return t;
}
