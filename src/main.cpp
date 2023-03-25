#include <iostream>
#include <string>
#include "../base/AsyncLog.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "../net/net.h"
#include <sys/socket.h>
#include "../base/threadpool.h"

void handler()
{
    std::cout << "handler func exec" << std::endl;
}

int intHandler(int a)
{
    std::cout << "input a:" << a << std::endl;
    return a;
}

int main(int argc, char *argv[])
{
#ifdef WIN32
    DWORD threadId = ::GetCurrentThreadId();
#else
    int threadId = syscall(SYS_gettid);
#endif
    using net::thread::ThreadPool;
    AsyncLog::init();
    ThreadPool pool1(4, 4);
    pool1.start();
    pool1.exec(std::function<void()>(std::move(handler)));
    auto a = pool1.exec(std::function<int(int)>(std::move(intHandler)), 100);
    std::cout << a.get() << std::endl;
    std::cout << "in main thread" << std::endl;
    pool1.stopForAllDone();
}