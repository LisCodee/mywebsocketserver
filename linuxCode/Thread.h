#include <pthread.h>
#include <functional>
#include <stdint.h>

namespace net
{
    namespace thread
    {
        /// @brief Linux pthread相关封装
        class Thread
        {
        public:
            Thread(void *(*func)(void *), void *arg, bool detach=false);
            virtual ~Thread();

            /// @brief 取消线程
            /// @return 
            int cancel();

            /// @brief 等待线程执行结束，join失败会使程序结束
            /// @return 指向线程返回值的指针，如果joinable为false，返回空指针
            void* join();

            /// @brief 判断线程是否能join
            /// @return 
            bool joinable() const {return !detached_;}

            /// @brief 判断两个线程是否相等
            /// @param t 
            /// @return 
            bool operator==(const Thread& t) const {return pthread_equal(threadId_, t.threadId_);}

            uint64_t getId() const {return static_cast<uint64_t>(threadId_);}

            Thread(const Thread& rhs)=delete;
            Thread& operator=(const Thread& t) = delete;
        private:
            pthread_t threadId_;
            bool detached_;
        };
    }
}