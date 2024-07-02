//
// Created by lang liu on 2024/6/30.
//

#ifndef POLL_HPP
#define POLL_HPP


#include "Socket.hpp"
#include <poll.h>
#include <iostream>
#include <memory>

class Poll {
public:
    Poll(const int num, const int port) : _num(num), _port(port), _isRunning(false) {
        Init();
    }

    void Loop() {
        _isRunning = true;
        while (_isRunning) {
            int timeout = -1;   //-1表示一直阻塞，直到有文件描述符就绪

            int ret = poll(_fds.get(), _num, timeout);

            switch (ret)
            {
            case 0:
                std::cout << "poll timeout" << std::endl;
                break;
            case -1:
                std::cout << "poll error" << std::endl;
                break;
            default:
                Handle();
                break;
            }
        }
    }

private:
     void Handle() {
        for (int i = 0; i < _num; ++i) {
            if(_fds[i].fd == -1)    continue;

            int fd = _fds[i].fd;
            short event = _fds[i].revents;

            if(event & POLLIN) {
                if(fd == _socket->Fd()) {
                    int newFd = _socket->Accept();

                    if(newFd < 0) continue;

                    std::cout << "new connection: " << newFd << std::endl;

                    int j;
                    for(j= 1; j < _num; ++j) {
                        if(_fds[j].fd == -1) {
                            _fds[j].fd = newFd;
                            _fds[j].events = POLLIN;
                            break;
                        }
                    }
                    if(j == _num) close(newFd);
                }
                else {
                    // 普通读事件
                    char buffer[1024]{};
                    ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
                    if(n > 0) {
                        std::cout << "client: " << buffer << std::endl;
                    }
                    else {
                        std::cout << "client quit or error" << std::endl;
                        close(fd);
                        _fds[i].fd = -1;
                        _fds[i].events = 0;
                    }
                }
            }
        }
    }

    void Init() {
        _socket = std::make_unique<Socket>();
        _socket->BuildListenSocket(_port);
        _fds = std::make_unique<pollfd[]>(_num);
        for(int i = 0; i < _num; ++i) {
            _fds[i].fd = -1;
            _fds[i].events = _fds[i].revents = 0;
        }

        _fds[0].fd = _socket->Fd();
        _fds[0].events = POLLIN;
    }

private:
    std::unique_ptr<Socket> _socket;
    std::unique_ptr<pollfd[]> _fds;

    int _num;
    int _port;
    bool _isRunning;
};

#endif //POLL_HPP
