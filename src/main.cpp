#include <iostream>
#include <string>
#include "../base/AsyncLog.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "../net/net.h"
#include <sys/socket.h>

int main(int argc, char *argv[])
{
#ifdef WIN32
    DWORD threadId = ::GetCurrentThreadId();
#else
    int threadId = syscall(SYS_gettid);
#endif
    std::cout << "main threadId:" << threadId << std::endl;
    AsyncLog::init("mainLog.log");
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    net::Socket s(sockfd);
    std::cout << "sockfd:" << s.fd() << std::endl;
    net::Socket s2(true);
    std::cout << "sock2fd:" << s2.fd() << std::endl;
    AsyncLog::uninit();
    return 0;
}