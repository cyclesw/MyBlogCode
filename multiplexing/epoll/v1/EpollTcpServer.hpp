//
// Created by lang liu on 2024/6/30.
//

#ifndef POLLSERVER_HPP
#define POLLSERVER_HPP

#include "Socket.hpp"
#include "Epoll.hpp"
#include "Connection.hpp"
#include <muduo/net/TcpConnection.h>
#include <unordered_map>

const static int max_events = 128;

class EpollServer {
public:
    EpollServer(int port)
        : _port(port), _socket(new Socket()),
          _epoller(new Epoller), _isRunning(false) { Init(); }

    void Loop() {
        _isRunning = true;
        while (_isRunning) {
            int timeout = -1;
            int ret = _epoller->Wait(_revs, max_events, timeout);
            switch (ret) {
                case 0:
                    std::cout << "timeout" << std::endl;
                    break;
                case -1:
                    std::cout << "epoll error" << std::endl;
                    break;
                default:
                    Handle(ret);
                    break;
            }
        }
    }

    bool Accepter() {
        int fd = _socket->Accept();
        if(fd < 0) return false;
        _epoller->AddEvent(fd, EPOLLIN);
        std::cout << "new connection:"  << fd << std::endl;
        return true;
    }

    int Recver(int fd, std::string* out) {
        char buffer[1024] {};
        int n =::recv(fd, buffer, sizeof(buffer), 0);
        *out += buffer;
        return n;
    }

private:
    void Handle(int n) {
        for(int i = 0; i < n; ++i) {
            int sockfd = _revs[i].data.fd;
            uint32_t events = _revs[i].events;

            if(events & EPOLLIN) {
                if(sockfd == _socket->Fd()) {
                    if (!Accepter())    continue;
                }
                else {
                    std::string msg;
                    int n = Recver(sockfd, &msg);
                    if(n > 0) {
                        std::cout << "client: " << msg << std::endl;
                        send(sockfd, msg.c_str(), msg.size(), 0);
                    }
                    else {
                        std::cout << "client quit or error: " << sockfd << std::endl;
                        _epoller->DelEvent(sockfd);
                        close(sockfd);
                    }
                }
            }
        }
    }

    void Init() {
        _socket->BuildListenSocket(_port);
        _epoller->AddEvent(_socket->Fd(), EPOLLIN);
    }

private:
    epoll_event _revs[max_events];
    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Epoller> _epoller;

    int _port;
    bool _isRunning;
};

#endif //POLLSERVER_HPP
