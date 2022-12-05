#pragma once

#include <thread>

#include "Controller.h"
#include "main.h"

class Connection {

public:
        Connection(int socket, Controller *cnt) noexcept;

        [[nodiscard]] bool operator()() const noexcept;

        ~Connection();

private:
        void deviceTask();

        void mobileTask();

        void task();

        bool sendString(std::string msg);

        bool recvString(std::string &buffer);

private:
        int m_socket;

        bool m_continue = true;

        Controller *m_controller;

        std::thread m_loopThread;

        constexpr static int m_kRecvBufSize = 1024;
};