#pragma once
#ifdef WIN32

#else
#include "../linuxCode/Thread.h"
#include "../linuxCode/sync.h"
#endif

#include <atomic>
#include <queue>
#include <memory>
#include <functional>
#include <vector>
#include <future>
namespace net
{
    namespace thread
    {
        class ThreadPool
        {
        public:
            typedef std::function<void()> Task;
            explicit ThreadPool(int numThreads, int maxThreads);
            virtual ~ThreadPool();
            /// @brief 启动线程池
            void start();
            /// @brief 当前正在执行的任务执行完后退出
            /// 谨慎使用！！！
            void stopForAllDone();
            /// @brief 获取任务
            /// @param t
            /// @return
            bool getTask(Task &t);

            template <class F, class... Args>
            auto exec(F &&f, Args... args) -> std::future<decltype(f(args...))>;

        public:
            static int kPoolNums; // 线程池数量

        private:
            /// @brief 线程BASE函数
            void threadLoop();
            static void *threadFunc(void *arg)
            {
                ThreadPool *obj = static_cast<ThreadPool *>(arg);
                obj->threadLoop();
                return nullptr;
            }

        private:
            int kPoolIdx_;                                         // 当前线程池编号
            std::atomic_bool bStop_;                               // 线程池是否停止
            std::atomic_uint32_t runningNum_;                      // 当前正在执行的任务数
            std::vector<std::shared_ptr<Thread>> listIdelThreads_; // 空闲先线程
            std::vector<std::shared_ptr<Thread>> listBusyThreads_; // 忙碌线程
            std::queue<Task> queueTasks_;                          // 任务队列
            Mutex mutex_;                                          // 保护任务队列互斥量
            ConditionVariable cvTask_;                             // 任务队列条件变量
            ConditionVariable cvExit_;                             // 退出条件变量
            int kCurrentThreadNum_;                                // 当前线程数
            int kMaxThreadNum_;                                    // 最大线程数
        };
        template <class F, class... Args>
        inline auto ThreadPool::exec(F &&f, Args... args) -> std::future<decltype(f(args...))>
        {
            if(bStop_)
            {
                start();
            }
            using RetType = decltype(f(args...)); // 封装返回值
            // 下面这行代码~~需要详细研究
            auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...)); // 封装任务
            Task t = [task]()
            {
                (*task)();
            };
            mutex_.lock();
            queueTasks_.push(t);
            cvTask_.notifyOne();
            mutex_.unlock();
            return task->get_future();
        }
    }
}
