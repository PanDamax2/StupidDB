
#pragma once

#include "TCP_Server.hpp"
#include <string>
#include <map>

#define BUFFER_SIZE 4096
#define SERVER_NAME "STDB HTTP Server"

enum class HTTP_Status {
    OK = 200,
    BadRequest = 400,
    Forbitten = 403,
    NotFound = 404,
    InternalServerError = 500
};

class HTTP_Request_Header {
public:
    HTTP_Request_Header(const std::string& rawHeader);
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
};

typedef struct {
    std::string header;
    std::string body;
} HTTP_Response;


class HTTP_Server : public TCP_Server {
private:
    Session sessionManager;
    static std::string createHTTPHeader(HTTP_Status status, std::map<std::string, std::string> headers);
    static HTTP_Response generateError(HTTP_Status status, const std::string& message);
    static HTTP_Response getHome();
    // Wypierdol tą linijkę poniżej potem
    static std::string testParser();
    HTTP_Response login(const std::string& body);
    HTTP_Response query(HTTP_Request_Header header, const std::string& body);
    HTTP_Response logout(HTTP_Request_Header header);
    void handleHTTPRequest(socket_t client_fd);
public:
    HTTP_Server(int port) : TCP_Server(port, [this](socket_t client_fd) { this->handleHTTPRequest(client_fd); }) {};
};