#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

class ConnectionManager {
private:
    std::string address;
    int port;
    std::string token;
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string *out);
    static size_t headerCallback(void* contents, size_t size, size_t nmemb, std::vector<std::string> *out);
    static void printTable(const std::vector<std::vector<std::string>>& table);
    static void curlCleanup(CURL* curl, struct curl_slist* headers);
public:
    ConnectionManager(const std::string& address, int port);
    ~ConnectionManager();
    bool login(const std::string& password);
    bool query(const std::string& query);
    bool logout();
    bool changePassword(const std::string& password);
};