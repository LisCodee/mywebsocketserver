#pragma once

#include <string>

#ifdef WIN32

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#define SOCKET int
#endif

namespace net
{
    namespace sockets
    {

        SOCKET createOrDie();
        SOCKET createNonblockOrDie();
        void setNonblockAndCloseOnExec(SOCKET sockfd);

        void setReuseAddr(SOCKET sockfd, bool on);
        void setReusePort(SOCKET sockfd, bool on);

        int connect(SOCKET sockfd, const struct sockaddr_in &addr);
        void bindOrDie(SOCKET sockfd, const struct sockaddr_in &addr);
        void listenOrDie(SOCKET sockfd);
        SOCKET accept(SOCKET sockfd, struct sockaddr_in *clientAddr);

        int32_t read(SOCKET sockfd, void *buf, int32_t count);
        int32_t write(SOCKET sockfd, const void *buf, int32_t count);
        void close(SOCKET sockfd);

        void toIpPort(char *buf, size_t size, const struct sockaddr_in &addr);
        void toIp(char *buf, size_t size, const struct sockaddr_in &addr);
        void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in *addr);
        struct sockaddr_in getLocalAddr(SOCKET sockfd);
        struct sockaddr_in getPeerAddr(SOCKET sockfd);
        int getSockError(SOCKET sockfd);

        /// @brief 设置TCP_NODELAY，禁用Nagle算法，从而减少网络延迟
        /// @param sockfd 需要设置的套接字fd
        /// @param on 是否设置
        void setTcpNoDelay(SOCKET sockfd, bool on);

        /// @brief 启用或禁用在面向连接的socket上发送保活消息。
        ///        这个选项可以用来检测对端是否仍然存活，以及是否能正常工作。
        /// @param sockfd
        /// @param on
        void setKeepAlive(SOCKET sockfd, bool on);

    }
    /// @brief sockaddr_in的封装
    class InetAddress
    {
    public:
        explicit InetAddress(uint16_t port, bool loopBackOnly = false);
        InetAddress(const std::string& ip, uint16_t port);
        InetAddress(const struct sockaddr_in& addr): addr_(addr){}

        InetAddress(const InetAddress& rhs) = delete;
        InetAddress& operator=(const InetAddress& rhs) = delete;

        std::string toIp() const;
        std::string toIpPort() const;
        uint16_t toPort() const;

        const struct sockaddr_in& getSockAddrIn() const {return addr_;}
        void setSockAddrIn(const struct sockaddr_in& addr){addr_ = addr;}
        uint32_t ipNetEndian() const {return addr_.sin_addr.s_addr;}
        uint16_t portNetEndian() const {return addr_.sin_port;}

        static bool resolveHostname(std::string hostname, InetAddress* result);
    private:
        struct sockaddr_in addr_;
    };

    /// @brief Socket类为socket的RAII封装
    class Socket
    {
    public:
        Socket(bool nonblock, int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
        explicit Socket(int sockfd) : sockfd_(sockfd) {}
        ~Socket() { net::sockets::close(sockfd_); }
        SOCKET fd() const { return sockfd_; }

        Socket(const Socket& rhs) = delete;
        Socket& operator=(const Socket& rhs) = delete;

        void bindAddress(const InetAddress& addr);
        int connect(const InetAddress& addr);
        void listen();
        SOCKET accept(InetAddress& addr);
        void setNonblock();
        void setReuseAddr(bool on);
        void setReusePort(bool on);

        void setTcpNoDelay(bool on);
        void setKeepAlive(bool on);

    private:
        SOCKET sockfd_;
    };
}