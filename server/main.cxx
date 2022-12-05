#include <iostream>
#include <thread>

#include "main.h"
#include "server.h"
#include "Controller.h"

Controller g_cnt;

int main()
{
        int res = 0;

        std::string portNumber = "1337";

        auto srv = std::make_unique<server::Server>(portNumber);
        srv->setController(&g_cnt);

        std::thread inThread(&server::Server::removeClosed, srv.get());

        if (srv->initialized()) {
                srv->waitConnections();
        } else {
                ERR("server initialization failture");
        }

        inThread.join();
        return res;
}
