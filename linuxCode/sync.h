#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>

namespace net
{
    namespace thread
    {
        class Mutex
        {
        public:
            Mutex() : mutex_(PTHREAD_MUTEX_INITIALIZER) {}
            ~Mutex() {}

            void lock();
            void unlock();
            int tryLock();
            pthread_mutex_t& getMutex() {return mutex_;}

            Mutex(const Mutex &lhr) = delete;
            Mutex &operator=(const Mutex &lhr) = delete;

        private:
            pthread_mutex_t mutex_;
        };

        class ConditionVariable
        {
        public:
            ConditionVariable():cv_(PTHREAD_COND_INITIALIZER){}
            ~ConditionVariable() {}
            /// @brief 超时等待
            /// @param m 
            /// @param timeval 以纳秒为单位
            /// @return 
            int timedWait(Mutex& m, uint64_t timeval);
            int wait(Mutex& m);
            int notifyOne();
            int notifyAll();

            ConditionVariable(const ConditionVariable& lhr) = delete;
            ConditionVariable& operator=(const ConditionVariable& lhr) = delete;
        private:
            pthread_cond_t cv_;
        };

        class Semaphore
        {
        public:
            Semaphore();
            ~Semaphore(){}

            int wait();
            int trywait();
            int timedWait(uint64_t time);
            int post();
        private:
            sem_t sem_;
        };
    }
}