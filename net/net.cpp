#include "net.h"
#include "../base/AsyncLog.h"
#include <sys/fcntl.h>
#include <cstring>

SOCKET net::sockets::createOrDie()
{
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        LOGF("create socket error:%s!", strerror(errno));
    return sockfd;
}

SOCKET net::sockets::createNonblockOrDie()
{
    SOCKET sockfd = createOrDie();
    setNonblockAndCloseOnExec(sockfd);
    return sockfd;
}

void net::sockets::setNonblockAndCloseOnExec(SOCKET sockfd)
{
#ifdef WIN32
#else
    int oldFlag = fcntl(sockfd, F_GETFL, 0);
    int newFlag = oldFlag | O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, newFlag) == -1)
    {
        LOGF("set nonblock to sockfd: %d error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
    }
#endif
}

void net::sockets::setReuseAddr(SOCKET sockfd, bool on)
{
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        LOGE("setReuseAddr to sockfd:%d error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
    }
}

void net::sockets::setReusePort(SOCKET sockfd, bool on)
{
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) == -1)
    {
        LOGE("setReusePort to sockfd:%d error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
    }
}

int net::sockets::connect(SOCKET sockfd, const sockaddr_in &addr)
{
    if (::connect(sockfd, (const sockaddr *)&addr, sizeof(addr)) == -1)
    {
        LOGE("sockfd:%d connect to addr error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
        return -1;
    }
    return 0;
}

void net::sockets::bindOrDie(SOCKET sockfd, const sockaddr_in &addr)
{
    if (::bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        char ip[32]{0};
        net::sockets::toIp(ip, sizeof(ip), addr);
        uint16_t port = ntohs(addr.sin_port);
        int err = net::sockets::getSockError(sockfd);
        LOGF("bind addr to sockfd:%d error:%s, addr:%s:%d", sockfd, strerror(err), ip, port);
    }
    LOGI("bind addr success, sockfd:%d", sockfd);
}

void net::sockets::listenOrDie(SOCKET sockfd)
{
    if (::listen(sockfd, 0) < 0)
    {
        LOGF("sockfd:%d listen error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
    }
}

SOCKET net::sockets::accept(SOCKET sockfd, sockaddr_in *clientAddr)
{
    socklen_t clientAddrLen = sizeof(clientAddr);
    SOCKET clientfd = ::accept(sockfd, (sockaddr *)clientAddr, &clientAddrLen);
    if (clientfd == -1)
    {
        LOGE("accept sock from sockfd:%d error:%s, accept failed", sockfd, strerror(net::sockets::getSockError(sockfd)));
        return -1;
    }
    return clientfd;
}

/// 暂时没考虑一次没读完的情况
int32_t net::sockets::read(SOCKET sockfd, void *buf, int32_t count)
{
    int32_t n = ::recv(sockfd, buf, count, 0);
    if (n == 0)
    {
        // 对端关闭连接
        close(sockfd);
        return 0;
    }
    else if (n == -1)
    {
        if (getSockError(sockfd) == EWOULDBLOCK)
        {
            // 阻塞
            LOGI("read blocked.");
        }
        else
        {
            // 出错
            LOGE("read from sockfd:%d error", sockfd);
        }
        return -1;
    }
    return n;
}

///暂时没有考虑一次没写完的情况
int32_t net::sockets::write(SOCKET sockfd, const void *buf, int32_t count)
{
    int32_t n = ::send(sockfd, buf, count, 0);
    if (n == -1)
    {
        if (getSockError(sockfd) == EWOULDBLOCK)
        {
            // 阻塞
            LOGI("read blocked.");
        }
        else
        {
            LOGE("read from sockfd:%d error", sockfd);
            // 出错
        }
        return -1;
    }
    return n;
}

void net::sockets::close(SOCKET sockfd)
{
#ifdef WIN32
    closesocket(sockfd);
#else
    ::close(sockfd);
#endif
}

void net::sockets::toIpPort(char *buf, size_t size, const sockaddr_in &addr)
{
    char ip[32]{0};
    toIp(ip, sizeof(ip), addr);
    uint16_t port = ntohs(addr.sin_port);
    snprintf(buf, size, "%s:%d", ip, port);
}

void net::sockets::toIp(char *buf, size_t size, const sockaddr_in &addr)
{
    char *ip = inet_ntoa(addr.sin_addr);
    strncpy(buf, ip, size);
}

void net::sockets::fromIpPort(const char *ip, uint16_t port, sockaddr_in *addr)
{
    inet_aton(ip, &addr->sin_addr);
    addr->sin_port = htons(port);
}

sockaddr_in net::sockets::getLocalAddr(SOCKET sockfd)
{
    sockaddr_in localAddr;
    socklen_t localAddrLen = sizeof(localAddr);
    getsockname(sockfd, (struct sockaddr *)&localAddr, &localAddrLen);
    return localAddr;
}

sockaddr_in net::sockets::getPeerAddr(SOCKET sockfd)
{
    sockaddr_in peerAddr;
    socklen_t peerAddrLen = sizeof(peerAddr);
    getpeername(sockfd, (struct sockaddr *)&peerAddr, &peerAddrLen);
    return peerAddr;
}

int net::sockets::getSockError(SOCKET sockfd)
{
    int err;
#ifdef WIN32
    err = WSAGetLastError();
#else
    err = errno;
#endif
    int val = 0, valLen = sizeof(val);
    if(err == EWOULDBLOCK && sockfd > 0)
    {
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &val, (socklen_t*)&valLen) == -1)
        {
            return err;
        }
        if(val)
            return val;
    }
    return err;
}

void net::sockets::setTcpNoDelay(SOCKET sockfd, bool on)
{
    if(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1)
    {
        LOGE("set tcp nodelay to sockfd:%d error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
    }
}

void net::sockets::setKeepAlive(SOCKET sockfd, bool on)
{
    if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) == -1)
    {
        LOGE("set socket:%d keep alive error:%s", sockfd, strerror(net::sockets::getSockError(sockfd)));
    }
}

net::Socket::Socket(bool nonblock, int domain, int type, int protocol)
{
    if((sockfd_ = ::socket(domain, type, protocol)) == -1)
    {
        LOGF("create socket error:%s", strerror(errno));
    }
    if(nonblock)
        net::sockets::setNonblockAndCloseOnExec(sockfd_);
}

void net::Socket::bindAddress(const InetAddress &addr)
{
    net::sockets::bindOrDie(sockfd_, addr.getSockAddrIn());
}

int net::Socket::connect(const InetAddress &addr)
{
    return net::sockets::connect(sockfd_, addr.getSockAddrIn());
}

void net::Socket::listen()
{
    net::sockets::listenOrDie(sockfd_);
}

SOCKET net::Socket::accept(InetAddress &addr)
{
    struct sockaddr_in cAddr;
    SOCKET clientfd =  net::sockets::accept(sockfd_, &cAddr);
    addr.setSockAddrIn(cAddr);
    LOGI("server accept a client:%s", addr.toIpPort());
    return clientfd;
}

uint32_t net::Socket::write(const void *buffer, uint32_t size)
{
    return net::sockets::write(sockfd_, buffer, size);
}

uint32_t net::Socket::read(void *buffer, uint32_t size)
{
    return net::sockets::read(sockfd_, buffer, size);
}

void net::Socket::setNonblock()
{
    net::sockets::setNonblockAndCloseOnExec(sockfd_);
}

void net::Socket::setReuseAddr(bool on)
{
    net::sockets::setReuseAddr(sockfd_, on);
}

void net::Socket::setReusePort(bool on)
{
    net::sockets::setReusePort(sockfd_, on);
}

void net::Socket::setTcpNoDelay(bool on)
{
    net::sockets::setTcpNoDelay(sockfd_, on);
}

void net::Socket::setKeepAlive(bool on)
{
    net::sockets::setKeepAlive(sockfd_, on);
}

net::InetAddress::InetAddress()
{
    memset(&addr_, 0, sizeof(addr_));
}

net::InetAddress::InetAddress(uint16_t port, bool loopBackOnly)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    in_addr_t ip = loopBackOnly ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_addr.s_addr = htonl(ip);
}

net::InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    net::sockets::fromIpPort(ip.c_str(), port, &addr_);
    addr_.sin_family = AF_INET;
}

std::string net::InetAddress::toIp() const
{
    char ip[32]{0};
    net::sockets::toIp(ip, sizeof(ip), addr_);
    return ip;
}

std::string net::InetAddress::toIpPort() const
{
    char ipPort[32]{0};
    net::sockets::toIpPort(ipPort, sizeof(ipPort), addr_);
    return ipPort;
}

uint16_t net::InetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}

bool net::InetAddress::resolveHostname(std::string hostname, InetAddress *result)
{
    struct addrinfo hint, *res, *p;
    memset(&hint, 0, sizeof(hint));
    if(getaddrinfo(hostname.c_str(), "http", &hint, &res) != 0)
    {
        LOGE("resolveHostname error, errno:%d, err:%s", errno, strerror(errno));
    }
    //获取第一个ipv4地址
    for(p = res; p != NULL;)
    {
        if(p->ai_family == AF_INET)
        {
            struct sockaddr_in *temp = (struct sockaddr_in*)p->ai_addr;
            result->setSockAddrIn(*temp);
            return true;
        }
        p = p->ai_next;
    }
    return false;
}
