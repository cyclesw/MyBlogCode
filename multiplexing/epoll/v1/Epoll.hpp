//
// Created by lang liu on 2024/6/30.
//

#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <iostream>

#include "Socket.hpp"
#include <sys/epoll.h>
#include <set>

const static int size = 128;

class Epoller {
public:
    Epoller() :_epfd(-1) {
        Init();
    }

    ~Epoller() { close(_epfd); }

    void AddEvent(const int fd, const uint32_t events) {
        _fdlist.insert(fd);

        epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;

        int n = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
        if(n < 0)   perror("epollAdd");
    }

    void DelEvent(const int fd) {
        if(_fdlist.find(fd) == _fdlist.end()) return;
        ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
    }

    void ModEvent(const int fd, const uint32_t events) {
        epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;

        if(::epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) < 0)
            perror("epollMod");
    }

    int Wait(epoll_event* ev, int maxevents, int timeout) {
        return ::epoll_wait(_epfd, ev,maxevents, timeout);
    }

private:
    void Init() {
        _epfd = ::epoll_create1(0);
        if(_epfd < 0) {
            std::cerr << "epoll_crete error" << std::endl;
            abort();
        }
    }


private:
    int _epfd;
    std::set<int> _fdlist;
};

#endif //EPOLL_HPP
