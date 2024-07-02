//
// Created by lang liu on 2024/6/29.
//

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>


const int backlog = 5;

class Socket {
public:
    Socket* Get() { return this; }

    Socket() = default;

    ~Socket() { Close(); }

    void BuildListenSocket(const int port)  {
        _port = port;

        _fd = socket(AF_INET, SOCK_STREAM, 0);
        if(_fd < 0) {
            perror("socket");
            abort();
        }

        Bind();
        Listen();
    }

    ssize_t Recv(std::string& str, const int size) const {
        char buffer[size];
        ssize_t n  = ::recv(_fd , buffer, size, 0);
        buffer[n] = '\0';
        str += buffer;
        return  n;
    }

    ssize_t Send(std::string& buf, const size_t size) const {
        return ::send(_fd, buf.data(), size, 0);
    }

    explicit Socket(const int fd) :_fd(fd) {}

    int Fd() const { return _fd; }

    void SetFd(const int fd) { _fd = fd; }

    int Accept() {
        int fd = ::accept(_fd, (sockaddr*)&_addr, &_len);
        _port = ntohs(_addr.sin_port);
        if(fd < 0) {
            perror("accept");
        }
        return fd;
    }

    void Listen() const {
        if(::listen(_fd, backlog) != 0) {
            perror("listen");
            abort();
        }
    }

    void Close() const {
        close(_fd);
    }

    void Bind() {
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = INADDR_ANY;
        _addr.sin_port = htons(_port);

        if(bind(_fd, (sockaddr*)&_addr, _len) != 0) {
            perror("bind");
            abort();
        }
    }

private:
    sockaddr_in _addr{};

    socklen_t _len = sizeof(_addr);
    int _port{};
    int _fd = -1;
};

#endif //SOCKET_HPP
