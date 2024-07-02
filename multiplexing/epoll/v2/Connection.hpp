//
// Created by lang liu on 2024/6/30.
//

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Socket.hpp"
#include "Epoll.hpp"

#include <memory>

class Connection {
public:
private:
    std::string _inBuffer;
    std::string _outBuffer;

    std::unique_ptr<Socket> _socket;
};

#endif //CONNECTION_HPP
