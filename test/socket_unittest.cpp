#include <gtest/gtest.h>
#include "../net/net.h"
#include "../base/AsyncLog.h"
#include <sys/socket.h>

namespace
{
    using net::InetAddress;
    using net::Socket;
    class SocketTest : public ::testing::Test
    {
    protected:
        static void SetUpTestSuite()
        {
            AsyncLog::init("");
        }
        static void TearDownTestSuite()
        {
            AsyncLog::uninit();
        }
        void SetUp() override
        {
            sock = new Socket(true);
        }

        void TearDown() override
        {
            delete sock;
        }

        Socket *sock;
    };

    using SocketDeathTest = SocketTest;

    /// @brief 测试Socket构造函数
    TEST_F(SocketDeathTest, TestSocketCtor)
    {
        Socket *clientSocket1;
        Socket *clientSocket2;
        Socket *serverSocket;
        int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        clientSocket1 = new Socket(sockfd);
        EXPECT_GT(clientSocket1->fd(), 0);
        clientSocket2 = new Socket(true);
        EXPECT_GT(clientSocket2->fd(), 0);
        serverSocket = new Socket(false);
        EXPECT_GT(serverSocket->fd(), 0) << "serverfd:" << serverSocket->fd();
        std::cout << "client1:" << clientSocket1->fd() << "\tclient2:"
                  << clientSocket2->fd() << "\tserver:" << serverSocket->fd() << std::endl;
        EXPECT_NE(clientSocket1->fd(), clientSocket2->fd());
        EXPECT_NE(clientSocket1->fd(), serverSocket->fd());
    }

    /// 测试server方法
    TEST_F(SocketDeathTest, TestSocketBind)
    {
        InetAddress *clientAddr1 = nullptr;
        InetAddress clientAddr2("127.0.0.1", 9999);
        InetAddress serverAddr(9000, true);
        std::cout << "bind client2" << std::endl;
        sock->bindAddress(clientAddr2);
        sock->listen();
        std::cout << "bind server" << std::endl;
        // 连续调用两次bind会导致segment fault
        EXPECT_DEATH(sock->bindAddress(serverAddr), "");
        // bind一个非法InetAddress
        Socket clientSocket1(true);
        std::cout << "bind client1" << std::endl;
        EXPECT_DEATH(clientSocket1.bindAddress(*clientAddr1), "");
    }

    //测试set相关方法
    TEST_F(SocketTest, TestSocketVoid)
    {
        sock->setNonblock();
        sock->setReuseAddr(true);
        sock->setReusePort(true);
        sock->setTcpNoDelay(true);
        sock->setKeepAlive(true);
    }

    ///测试client相关方法
    TEST_F(SocketTest, TestSocketClient)
    {
        sock->setReuseAddr(true);
        sock->setReusePort(true);
        InetAddress server(10000, true);
        sock->bindAddress(server);
        sock->listen();
        Socket client(true);
        client.connect(server);
        InetAddress clientAddr;
        SOCKET cfd = sock->accept(clientAddr);
        Socket clientRet(cfd);
        std::cout << "accept a sock:" << cfd << std::endl;
        EXPECT_GT(cfd, 0);
        char data[] = "data";
        uint32_t dataLen = sizeof(data);
        EXPECT_EQ(client.write((void*)data, dataLen), dataLen);
        char recv[32]{0};
        uint32_t recvLen = sizeof(recv);
        EXPECT_EQ(clientRet.read(recv, recvLen), dataLen);
        std::cout << "recv from client:" << recv << std::endl;
        EXPECT_EQ(strcmp(data, recv), 0);
    }
}
