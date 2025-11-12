#include "../include/HTTP_Server.hpp"
#include "../include/Session.hpp"
#include <iostream>

int main() {
    HTTP_Server server(DEFAULT_PORT);
    server.start();

    return 0;
}