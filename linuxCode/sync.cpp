#include "sync.h"
#include <stdint.h>
#include "../base/time.h"

void net::thread::Mutex::lock()
{
    pthread_mutex_lock(&mutex_);
}

void net::thread::Mutex::unlock()
{
    pthread_mutex_unlock(&mutex_);
}

int net::thread::Mutex::tryLock()
{
    return pthread_mutex_trylock(&mutex_);
}

int net::thread::ConditionVariable::timedWait(Mutex &m, uint64_t timeval)
{
    timespec tv = {timeval / TimeStamp::NanosecondsPerSecond,
                   timeval % TimeStamp::NanosecondsPerSecond};
    return pthread_cond_timedwait(&cv_, &m.getMutex(), &tv);
}

int net::thread::ConditionVariable::wait(Mutex &m)
{
    return pthread_cond_wait(&cv_, &m.getMutex());
}

int net::thread::ConditionVariable::notifyOne()
{
    return pthread_cond_signal(&cv_);
}

int net::thread::ConditionVariable::notifyAll()
{
    return pthread_cond_broadcast(&cv_);
}

net::thread::Semaphore::Semaphore()
{
    sem_init(&sem_, 0, 1);
}

int net::thread::Semaphore::wait()
{
    return sem_wait(&sem_);
}

int net::thread::Semaphore::trywait()
{
    return sem_trywait(&sem_);
}

int net::thread::Semaphore::timedWait(uint64_t time)
{
    timespec tv{time / TimeStamp::NanosecondsPerSecond,
                time % TimeStamp::NanosecondsPerSecond};
    return sem_timedwait(&sem_, &tv);
}

int net::thread::Semaphore::post()
{
    return sem_post(&sem_);
}
