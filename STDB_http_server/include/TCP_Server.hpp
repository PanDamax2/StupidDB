#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

#include <functional>

#include "Session.hpp"

#define DEFAULT_PORT 8080

#ifdef _WIN32
    using socket_t = SOCKET;
#else
    using socket_t = int;
#endif

class TCP_Server {
private:
    int port;
    std::function<void(socket_t client_fd)> requestHandler;
    void handleRequest(socket_t client_fd, sockaddr_in client_addr);
public:
    TCP_Server(int port, std::function<void(socket_t client_fd)> handler);
    void start();
};