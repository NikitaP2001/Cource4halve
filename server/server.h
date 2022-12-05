#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <string>

#include "connection.h"
#include "main.h"

namespace server {

class Server {

public:
        Server(std::string port) noexcept;
        ~Server();

        void setController(Controller *cnt);

        bool waitConnections() noexcept;

        [[nodiscard]] bool initialized() const noexcept;

        void removeClosed() noexcept;

private:

        bool m_initialized = true;

        std::mutex m_connectionsMutex;
        std::list<std::unique_ptr<Connection>> m_connections;

        Controller *m_controller = nullptr;

        bool m_isRunning = true;

        int m_listenSocket = -1;

        constexpr static int m_sockTimeoutSec = 10;
};

} // server