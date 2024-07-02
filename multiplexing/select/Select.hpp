//
// Created by lang liu on 2024/6/29.
//

#ifndef SELECT_HPP
#define SELECT_HPP


#include "Socket.hpp"
#include <sys/select.h>
#include <iostream>
#include <vector>

const int selectNum = 10;   //select 最大管理的文件描述符数量。

class Select {
public:
    explicit Select(int port) :_socket(port) {
        Init(port);
    }

    void Handle() {
        for(int i = 0; i < _array.size(); ++i) {
            if(!_array[i])  continue;

            int fd = _array[i]->Fd();

            if(FD_ISSET(fd, &_rfdset)) {
                //  新连接到来的情况
                if(fd == _socket.Fd()) {
                    Socket* socket = new Socket(_socket.Accept());

                    int j;
                    for(j = 1; j < _array.size(); ++j) {
                        if(!_array[j]) {
                            _array[j] = socket;
                            std::cout << "New Connection:" << socket->Fd() << std::endl;
                            break;
                        }
                    }

                    if(j == _array.size()) delete socket;
                }
                else {
                    //正常文件读取
                    std::string buf;
                    ssize_t sz = _array[i]->Recv(buf, 1024);
                    if(sz > 0) {
                        std::cout << "client: " << buf << std::endl;
                        _array[i]->Send(buf, sz);
                    }
                    else {
                        std::cout << "client error or quit:" << _array[i]->Fd() << std::endl;
                        FD_CLR(_array[i]->Fd(), &_rfdset);
                        delete _array[i];
                        _array[i] = nullptr;
                    }
                }
            }
        }
    }

    void Loop() {   //程序入口
        _isRunning = true;
        while (_isRunning) {
            FD_ZERO(&_rfdset);  // fd_set 每次使用前都要初始化
            int maxfd = _socket.Fd();

            for(int i = 0; i < _array.size(); ++i) {
                if(!_array[i])  continue;

                int fd = _array[i]->Fd();
                FD_SET(fd, &_rfdset);
                maxfd = fd > maxfd ? fd : maxfd;
            }

            timeval timeout = {1, 0};

            int n = select(maxfd+1, &_rfdset, nullptr, nullptr, &timeout);
            switch (n) {
                case 0:
                    std::cout << "select timeout, last time: " << timeout.tv_sec << ":" << timeout.tv_usec << std::endl;
                    break;
                case -1:
                    std::cerr << "select error!!!" << std::endl;
                    break;
                default:
                    Handle();
                    break;
            }
        }
    }

    void Stop() { _isRunning = false; }

private:
    void Init(int port) {
        _socket.BuildListenSocket(port);
        _array.resize(selectNum);
        for (int i = 0; i < _array.size(); ++i)
            _array[i] = nullptr;

        _array[0] = _socket.Get();
    }

private:
    Socket _socket;
    fd_set _rfdset;
    bool _isRunning{};

    std::vector<Socket*> _array{};  //使用vector来管理文件描述符
};



#endif //SELECT_HPP
