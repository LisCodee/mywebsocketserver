#include <pthread.h>

namespace net
{
    namespace thread
    {
        class Mutex
        {
        public:
            Mutex():mutex_(PTHREAD_MUTEX_INITIALIZER){}
            ~Mutex(){}

            void lock();
            void unlock();
            int tryLock();

            Mutex(const Mutex& lhr) = delete;
            Mutex& operator=(const Mutex& lhr) = delete;
        private:
            pthread_mutex_t mutex_;
        };
    }
}