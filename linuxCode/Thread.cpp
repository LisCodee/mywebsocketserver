#include "Thread.h"
#include "../base/AsyncLog.h"
#include <string.h>
#include <errno.h>

net::thread::Thread::Thread(void *(*func)(void *), void *arg, bool detach) : detached_(detach)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(
        &attr, detach ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
    if(pthread_create(&threadId_, &attr, func, arg) != 0)
    {
        LOGF("create thread error");
    }
    pthread_attr_destroy(&attr);
}

net::thread::Thread::~Thread()
{
}

int net::thread::Thread::cancel()
{
    detached_ = true;
    return pthread_cancel(threadId_);
}

void *net::thread::Thread::join()
{
    void *retVal;
    if(!this->joinable())
    {
        LOGW("target join thread:%ld is not joinable", threadId_);
        return (void*)0;
    }
    if(pthread_join(this->threadId_, &retVal) != 0)
    {
        LOGF("join to target thread:%ld error: %s", threadId_, strerror(errno));
    }
    return retVal;
}
