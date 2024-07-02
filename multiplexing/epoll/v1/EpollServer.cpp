//
// Created by lang liu on 2024/6/30.
//

#include "Epoll.hpp"
#include  "EpollTcpServer.hpp"

int main() {
    EpollServer svr(8888);
    svr.Loop();

    return 0;
}