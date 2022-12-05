#include <fcntl.h>

#include "server.h"

using namespace server;

static bool set_socket_blocking_enabled(int fd, bool blocking)
{
        if (fd < 0) return false;
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1)
                return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
}

Server::Server(std::string port) noexcept
{
        int status = 1;
        addrinfo *result = nullptr;
        addrinfo hints {};

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        status = getaddrinfo(NULL, port.data(), &hints, &result);
        if (status != 0) {
                ERR("getaddrinfo failed");
                m_initialized = false;
        }

        if (initialized()) {
                m_listenSocket = socket(result->ai_family, result->ai_socktype,
                result->ai_protocol);
                if (m_listenSocket == -1) {
                        ERR("socket creation failed");
                        freeaddrinfo(result);
                        m_initialized = false;
                }
        }

        if (initialized()) {
                status = bind(m_listenSocket, result->ai_addr,
                                result->ai_addrlen);
                if (status == -1) {
                        ERR("bind failed");
                        freeaddrinfo(result);
                        m_initialized = false;
                }
        }

        if (initialized()) {
                status = listen(m_listenSocket, SOMAXCONN);
                if (status == -1) {
                        ERR("listen failed");
                        m_initialized = false;
                }
                freeaddrinfo(result);
        }

}


void Server::setController(Controller *cnt)
{
        m_controller = cnt;
}


void Server::removeClosed() noexcept
{
        while (m_isRunning) {
                {
                        std::scoped_lock lck(m_connectionsMutex);
                        m_connections.remove_if([](auto &c)
                                        { return !(*c.get())(); });

                }
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
}


bool Server::waitConnections() noexcept
{
        bool status = true;
        struct timeval tv;
        tv.tv_sec = m_sockTimeoutSec;
        tv.tv_usec = 0;
        int clientSocket {};

        while (status) {
                INFO("accepting new");
                clientSocket = accept(m_listenSocket, NULL, NULL);
                if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
                                (const char*)&tv, sizeof tv) == -1) {
                        close(clientSocket);
                        clientSocket = -1;
                        ERR("setsockopt fail");
                }
                if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO,
                                (const char*)&tv, sizeof tv) == -1) {
                        close(clientSocket);
                        clientSocket = -1;
                        ERR("setsockopt fail");
                }
                if (clientSocket == -1) {
                        ERR("accept failed");
                        status = false;
                } else {
                        std::scoped_lock lck(m_connectionsMutex);
                        auto newConnection = std::make_unique<
                        Connection>(clientSocket, m_controller);
                        m_connections.push_back(std::move(newConnection));
                }
        }

        return status;
}


Server::~Server()
{
        m_isRunning = false;
        if (m_listenSocket != -1)
                close(m_listenSocket);
}


bool Server::initialized() const noexcept
{
        return m_initialized;
}