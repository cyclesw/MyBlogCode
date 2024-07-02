//
// Created by lang liu on 2024/6/30.
//

#include <sys/epoll.h>
#include "Socket.hpp"

int main() {
    Socket sockfd;

    sockfd.BuildListenSocket(8888);

    int efd = epoll_create1(0);

    epoll_event ev{};
    ev.data.fd = sockfd.Fd();
    ev.events = EPOLLIN;

    epoll_ctl(efd, EPOLL_CTL_ADD, sockfd.Fd(), &ev);

    while (1) {
        int ret = epoll_wait(efd, &ev, 1, -1);
        if(re)
    }

    return 0;
}