#include <mutex>
#include <cstring>
#include <sstream>

#include "connection.h"

Connection::Connection(int socket, Controller *cnt) noexcept
        : m_socket(socket),
        m_loopThread(&Connection::task, this),
        m_controller(cnt)
{

}


Connection::~Connection()
{
        m_loopThread.join();
        if (m_socket != -1) {
                close(m_socket);
        }
        INFO("connection removed");
}


bool Connection::operator()() const noexcept
{
        return m_continue;
}

bool Connection::sendString(std::string msg)
{
        int iResult = 0;
        bool result = true;

        iResult = send(m_socket, msg.c_str(), msg.size() + 1, 0);
        if (iResult == -1 || iResult != msg.size() + 1) {
                ERR("send failed ");
                result = false;
        }
        INFO("send " << msg);
        return result;
}

bool Connection::recvString(std::string &buffer)
{
        int iResult;
        bool msgEnd = false;
        std::unique_ptr<char[]> recvBuf =
                std::make_unique<char[]>(m_kRecvBufSize);
        buffer.clear();
        do {
                iResult = recv(m_socket, recvBuf.get(), m_kRecvBufSize, 0);
                if (iResult > 0) {
                        if (recvBuf[iResult - 1] == '\0')
                                msgEnd = true;
                        else
                                recvBuf[iResult] = '\0';
                        buffer += recvBuf.get();
                } else if (iResult == 0) {
                        msgEnd = true;
                        break;
                } else if (iResult == -1) {
                        ERR("recv failed " << strerror(errno));
                        break;
                }
        } while(!msgEnd);
        INFO("recv " << buffer);
        return msgEnd;
}

void Connection::deviceTask()
{
        INFO("device task");
        int currTemp = 0;
        int currHum = 0;
        bool connected = true;
        std::string response;

        while (connected) {
                std::stringstream ss;
                ss << m_controller->m_targetTemp << " "
                        <<  m_controller->m_targetHum;

                connected = sendString(ss.str());
                if (!connected)
                        break;

                connected = recvString(response);
                if (!connected)
                        break;

                sscanf(response.c_str(), "%d %d", &currTemp, &currHum);
                m_controller->m_temp = currTemp;
                m_controller->m_hum = currHum;

        }
        INFO("device disconnected");
}


void Connection::mobileTask()
{
        INFO("mobile task");
        std::string buffer;

        if (recvString(buffer)) {
                if (buffer == "get") {
                        buffer.clear();
                        buffer += std::to_string(m_controller->getTemp());
                        buffer.clear();
                        sendString(buffer);
                        buffer += std::to_string(m_controller->getHum());
                        sendString(buffer);

                } else if (buffer == "set") {
                        std::string temp, hum;
                        if (recvString(temp) && recvString(hum)) {
                                m_controller->setTargetTemp
                                        (std::stoi(temp));
                                m_controller->setTargetHum(std::stoi(hum));
                        }
                }
        }

        INFO("mobile disconnected");
}


void Connection::task()
{
        std::string msg;
        if (recvString(msg)) {
                if (msg == "Device")
                        deviceTask();
                else if (msg == "mobile")
                        mobileTask();
                else
                        ERR("protocol error");
        }

        if (shutdown(m_socket, SHUT_WR) == -1)
                ERR("shutdown failed");

        m_continue = false;
}