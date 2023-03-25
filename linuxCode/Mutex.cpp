#include "Mutex.h"

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
