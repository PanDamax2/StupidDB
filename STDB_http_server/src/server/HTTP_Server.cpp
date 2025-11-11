#include "../../include/HTTP_Server.hpp"
#include "../../include/Logger.hpp"

#include "../../libs/json.hpp"

using json = nlohmann::json;

HTTP_Request_Header::HTTP_Request_Header(const std::string& rawHeader) {
    size_t pos = 0;
    size_t lineEnd = rawHeader.find("\r\n");

    // Parse request line
    std::string requestLine = rawHeader.substr(0, lineEnd);
    size_t methodEnd = requestLine.find(' ');
    method = requestLine.substr(0, methodEnd);
    size_t pathEnd = requestLine.find(' ', methodEnd + 1);
    path = requestLine.substr(methodEnd + 1, pathEnd - methodEnd - 1);

    pos = lineEnd + 2;

    // Parse headers
    while ((lineEnd = rawHeader.find("\r\n", pos)) != std::string::npos && lineEnd != pos) {
        std::string headerLine = rawHeader.substr(pos, lineEnd - pos);
        size_t colonPos = headerLine.find(':');
        if (colonPos != std::string::npos) {
            std::string headerName = headerLine.substr(0, colonPos);
            std::string headerValue = headerLine.substr(colonPos + 1);
            // Trim whitespace
            headerValue.erase(0, headerValue.find_first_not_of(" "));
            headers[headerName] = headerValue;
        }
        pos = lineEnd + 2;
    }
}

std::string HTTP_Server::createHTTPHeader(HTTP_Status status, std::map<std::string, std::string> headers) {
    std::string header;

    // Status line
    header += "HTTP/1.1 " + std::to_string(static_cast<int>(status)) + " ";

    if(status < HTTP_Status::OK || status > HTTP_Status::InternalServerError) {
        status = HTTP_Status::InternalServerError;
    }

    if(status != HTTP_Status::OK) {
        Logger::warn("Blad HTTP: " + std::to_string(static_cast<int>(status)));
    }

    switch(status) {
        case HTTP_Status::OK:
            header += "OK\r\n";
            break;
        case HTTP_Status::BadRequest:
            header += "Bad Request\r\n";
            break;
        case HTTP_Status::Forbitten:
            header += "Forbidden\r\n";
            break;
        case HTTP_Status::NotFound:
            header += "Not Found\r\n";
            break;
        case HTTP_Status::InternalServerError:
            header += "Internal Server Error\r\n";
            break;
        default:
            header += "Unknown Status\r\n";
            break;
    }

    // Headers
    for(const auto& pair : headers) {
        header += pair.first + ": " + pair.second + "\r\n";
    }

    header += "\r\n"; // End of headers

    return header;
}

HTTP_Response HTTP_Server::generateError(HTTP_Status status, const std::string& message) {
    json responseJson;
    responseJson["error"] = static_cast<int>(status);
    responseJson["message"] = message;


    HTTP_Response response;
    response.body = responseJson.dump();

    std::map<std::string, std::string> headers;
    headers["Server"] = SERVER_NAME;
    headers["Content-Type"] = "text/json";
    headers["Content-Length"] = std::to_string(response.body.length());
    headers["Connection"] = "close";
 
    response.header = createHTTPHeader(status, headers);
    return response;
}


HTTP_Response HTTP_Server::getHome() {
    HTTP_Response response;

    response.body = "<!DOCTYPE html>\
<html><head><title>Stupid DB</title></head>\
<body><h1>Witamy w bazie danych Stupid DB!</h1></body></html>";

    std::map<std::string, std::string> headers;
    headers["Server"] = SERVER_NAME;
    headers["Content-Type"] = "text/html";
    headers["Content-Length"] = std::to_string(response.body.length());
    headers["Connection"] = "close";
 
    response.header = createHTTPHeader(HTTP_Status::OK, headers);
    return response;
}

HTTP_Response HTTP_Server::login(const std::string& body) {
    HTTP_Response response;
    json requestJson;

    try {
        requestJson = json::parse(body);
    } catch (json::parse_error& e) {
        Logger::warn("Nieudana proba logowania: niepoprawny format JSON.");
        return generateError(HTTP_Status::BadRequest, "Invalid JSON format in request body.");
    }

    if(!requestJson.contains("password")) {
        Logger::warn("Nieudana proba logowania: brak pola 'password'.");
        return generateError(HTTP_Status::BadRequest, "Missing 'password' field in request.");
    }

    std::string password = requestJson["password"];
    std::string token = sessionManager.createSession(password);
    if(token.empty()) {
        return generateError(HTTP_Status::Forbitten, "Invalid password.");
        Logger::warn("Nieudana proba logowania.");
    }

    Logger::info("Uzytkownik zalogowany pomyslnie.");

    json responseJson;
    responseJson["token"] = token;
    response.body = responseJson.dump();

    std::map<std::string, std::string> headers;
    headers["Server"] = SERVER_NAME;
    headers["Content-Type"] = "text/json";
    headers["Content-Length"] = std::to_string(response.body.length());
    headers["Connection"] = "close";

    response.header = createHTTPHeader(HTTP_Status::OK, headers);
    return response;
}

//  Wypierdol to co jest poniżej potem
std::string HTTP_Server::testParser() {
    json j;
    j["request_type"] = "data_query";
    j["cols"] = { "id", "name", "age", "isStudent" };
    j["data"] = {
        { 1, "Angel", 2137, false },
        { 2, "Bożydar", 25, false },
        { 3, "Klara", 35 , true}
    };

    return j.dump();
}

HTTP_Response HTTP_Server::query(HTTP_Request_Header header, const std::string& body) {
    HTTP_Response response;

    if(!sessionManager.isValidSession(header.headers["Token"])) {
        Logger::warn("Nieudana proba zapytania: niepoprawny token sesji.");
        return generateError(HTTP_Status::Forbitten, "Invalid session token.");
    }

    json queryJson;
    try {
        queryJson = json::parse(body);
    } catch (json::parse_error& e) {
        Logger::warn("Nieudana proba zapytania: niepoprawny format JSON.");
        return generateError(HTTP_Status::BadRequest, "Invalid JSON format in request body.");
    }

    if(!queryJson.contains("query")) {
        Logger::warn("Nieudana proba zapytania: brak pola 'query'.");
        return generateError(HTTP_Status::BadRequest, "Missing 'query' field in request.");
    }

    std::string query = queryJson["query"];
    Logger::info("Otrzymano zapytanie: " + query);

    // Wypierdol to potem. Tu będzie prawdziwy parser
    response.body = testParser();

    std::map<std::string, std::string> headers;
    headers["Server"] = SERVER_NAME;
    headers["Content-Type"] = "text/json";
    headers["Content-Length"] = std::to_string(response.body.length());
    headers["Connection"] = "close";

    response.header = createHTTPHeader(HTTP_Status::OK, headers);
    return response;
}

HTTP_Response HTTP_Server::logout(HTTP_Request_Header header) {
    HTTP_Response response;

    if(!sessionManager.isValidSession(header.headers["Token"])) {
        Logger::warn("Nieudana proba wylogowania: niepoprawny token sesji.");
        return generateError(HTTP_Status::Forbitten, "Invalid session token.");
    }

    sessionManager.destroySession(header.headers["Token"]);
    Logger::info("Uzytkownik wylogowany pomyslnie.");

    json responseJson;
    responseJson["message"] = "Logged out successfully.";

    response.body = responseJson.dump();

    std::map<std::string, std::string> headers;
    headers["Server"] = SERVER_NAME;
    headers["Content-Type"] = "text/json";
    headers["Content-Length"] = std::to_string(response.body.length());
    headers["Connection"] = "close";

    response.header = createHTTPHeader(HTTP_Status::OK, headers);
    return response;
}

void HTTP_Server::handleHTTPRequest(socket_t client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    int bytesRead = recv(client_fd, buffer, BUFFER_SIZE, 0);
    std::string bytesReadString = std::string(buffer, bytesRead);

    int endStr = bytesReadString.find("\r\n\r\n");
    std::string requestHeaderStr = bytesReadString.substr(0, endStr);
    std::string requestBody = bytesReadString.substr(endStr + 4);

    HTTP_Request_Header requestHeader(requestHeaderStr);

    if(bytesRead < 0) {
        Logger::error("Blad podczas odbierania danych od klienta.");
        return;
    }

    if(bytesRead == 0) {
        Logger::warn("Klient rozlaczyl się bez wysłania danych.");
        return;
    }

    Logger::info("Otrzymano zadanie: " + requestHeader.method + " " + requestHeader.path);
    HTTP_Response httpResponse;

    //Routing
    if(requestHeader.path == "/") {
        httpResponse = getHome();
    } else if(requestHeader.path == "/login") {
        httpResponse = login(requestBody);
    } else if(requestHeader.path == "/query") {
        httpResponse = query(requestHeader, requestBody);
    } else if(requestHeader.path == "/logout") {
        httpResponse = logout(requestHeader);
    } else {
        httpResponse = generateError(HTTP_Status::NotFound, "The requested resource was not found on this server.");
    }

    std::string response = httpResponse.header;
    if(requestHeader.method != "HEAD") {
        response += httpResponse.body;
    }
    
    send(client_fd, response.c_str(), response.size(), 0);
}