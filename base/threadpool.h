#pragma once

#include <thread>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>

class Task
{
public:
    Task();
    Task(void *data);
    virtual ~Task();

    void *getData() {return data_;}
    void setData(void *data){data_ = data; data = nullptr;}

    virtual void run() = 0;
    virtual void destroy() = 0;

private:
    void *data_;
};

class ThreadPool
{
public:
    static int kPoolNums; // 线程池数量
    typedef std::function<void()> Task;
    explicit ThreadPool(int numThreads, int maxThreads);
    virtual ~ThreadPool();

    void start();
    void stop();

    void addTask(const Task &t);

private:
    void threadLoop();
    Task getTask();

private:
    int kPoolIdx_;                                          // 当前线程池编号
    bool bStart_;                                           // 线程池是否启动
    bool bStop_;                                            // 线程池是否停止
    std::vector<std::shared_ptr<std::thread>> listThreads_; // 已创建的线程
    std::queue<Task> queueTasks_;                           // 任务队列
    std::mutex mutexTask_;                                  // 保护任务队列互斥量
    std::condition_variable cvTask_;                        // 任务队列条件变量
    int kCurrentThreadNum_;                                 // 当前线程数
    int kMaxThreadNum_;                                     // 最大线程数
};