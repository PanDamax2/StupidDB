#include <iostream>
#include <algorithm>

#include "../libs/json.hpp"

#include "../include/ConnectionManager.hpp"
#include "../include/Logger.hpp"

using json = nlohmann::json;

size_t ConnectionManager::writeCallback(void* contents, size_t size, size_t nmemb, std::string *out) {
    size_t totalSize = size * nmemb;
    out->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t ConnectionManager::headerCallback(void* contents, size_t size, size_t nmemb, std::vector<std::string> *out) {
    size_t totalSize = size * nmemb;
    out->emplace_back(static_cast<char*>(contents), totalSize);
    return totalSize;
}

//Zazolc gesla jazn

void  ConnectionManager::printTable(const std::vector<std::vector<std::string>>& table) {
     // policz szerokosci kolumn
    std::vector<size_t> widths(table[0].size(), 0);

    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); i++)
            widths[i] = std::max(widths[i], row[i].size());
    }

    auto printSeparator = [&]() {
        for (size_t w : widths) {
            std::cout << "+" << std::string(w + 2, '-');
        }
        std::cout << "+\n";
    };

    printSeparator();

    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); i++) {
            std::cout << "| " << row[i]
                      << std::string(widths[i] - row[i].size(), ' ')
                      << " ";
        }
        std::cout << "|\n";
        printSeparator();
    }
}

void ConnectionManager::curlCleanup(CURL* curl, struct curl_slist* headers) {
    if (headers) {
        curl_slist_free_all(headers);
    }
    if (curl) {
        curl_easy_cleanup(curl);
    }
}



ConnectionManager::ConnectionManager(const std::string& address, int port) : address(address), port(port) {
    // Inicjalizacja libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

ConnectionManager::~ConnectionManager() {
    // Czyszczenie libcurl
    curl_global_cleanup();
}

bool ConnectionManager::login(const std::string& password) {
    if(password.empty()) {
        Logger::error("Haslo nie moze byc puste.");
        return false;
    }               

    CURL* curl = curl_easy_init();
    if (!curl) {
        Logger::error("Initializacja CURL nie powiodla sie.");
        return false;
    }

    json jsonBody;
    jsonBody["password"] = password;

    std::string body = jsonBody.dump();

    std::string url = "http://" + address + ":" + std::to_string(port) + "/login";
    std::string response;
    std::vector<std::string> responseHeaders;


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);



    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        Logger::error("Blad podczas wykonywania zadania CURL: " + std::string(curl_easy_strerror(res)));
        curlCleanup(curl, nullptr);
        return false;
    }

    if(std::find(responseHeaders.begin(), responseHeaders.end(), "Server: STDB HTTP Server\r\n") == responseHeaders.end()) {
        Logger::error("Serwer jest nieprawidlowy.");
        curlCleanup(curl, nullptr);
        return false;
    }

    // Zakladamy, ze odpowiedz zawiera token w przypadku sukcesu
    if (response.empty()) {
        Logger::error("Pusta odpowiedz od serwera.");
        curlCleanup(curl, headers);
        return false;
    }
  

    try {
        json jsonResponse = json::parse(response);
        if(jsonResponse["resType"] == "ERROR") {
            Logger::error(jsonResponse["message"].get<std::string>());
            curlCleanup(curl, headers);
            return false;
        } else if (jsonResponse["resType"] == "TOKEN") {
            token = jsonResponse["token"].get<std::string>();
            Logger::info("Zalogowano pomyslnie. Token sesji otrzymany.");
            curlCleanup(curl, headers);
            return true;
        } else {
            Logger::error("Serwerowi odwala");
        }
    } catch (const json::parse_error& e) {
        Logger::error("Blad parsowania odpowiedzi JSON: " + std::string(e.what()));
        curlCleanup(curl, headers);
        return false;
    }
;

    curlCleanup(curl, headers);
    return false;

}

bool ConnectionManager::query(const std::string& query) {
    if(token.empty()) {
        Logger::error("Brak tokenu sesji. Zaloguj sie najpierw.");
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        Logger::error("Initializacja CURL nie powiodla sie.");
        return false;
    }

    json jsonBody;
    jsonBody["query"] = query;

    std::string body = jsonBody.dump();

    std::string url = "http://" + address + ":" + std::to_string(port) + "/query";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers,  ("Token: " + token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        Logger::error("Blad podczas wykonywania zadania CURL: " + std::string(curl_easy_strerror(res)));
        curlCleanup(curl, headers);
        return false;
    }

    // Zakladamy, ze odpowiedz zawiera token w przypadku sukcesu
    if (response.empty()) {
        Logger::error("Pusta odpowiedz od serwera.");
        curlCleanup(curl, headers);
        return false;
    }

    try {
        json jsonResponse = json::parse(response);
        if(jsonResponse["resType"] == "ERROR") {
            Logger::error(jsonResponse["message"].get<std::string>());
            curlCleanup(curl, headers);
            return false;
        } else if (jsonResponse["resType"] == "MESSAGE") {
            Logger::info(jsonResponse["message"].get<std::string>());
            curlCleanup(curl, headers);
            return true;
        } else if(jsonResponse["resType"] == "TABLE") {
            std::vector<std::vector<std::string>> tableToPrint;
            std::vector<std::string> cols = jsonResponse["cols"].get<std::vector<std::string>>();
            tableToPrint.push_back(cols);
            for(const auto& row : jsonResponse["rows"]) {
                
                std::vector<std::string> cells;
                for(const auto& cell : row) {
                    cells.push_back(cell.dump());
                }
                tableToPrint.push_back(cells);
            }
            printTable(tableToPrint);
            curlCleanup(curl, headers);
            return true;
        } else {
            Logger::error("Serwerowi odwala");
        }
    } catch (const json::parse_error& e) {
        Logger::error("Blad parsowania odpowiedzi JSON: " + std::string(e.what()));
        curlCleanup(curl, headers);
        return false;
    }

    curlCleanup(curl, headers);

    return false;
}

bool ConnectionManager::logout() {
    if(token.empty()) {
        Logger::error("Brak tokenu sesji. Zaloguj sie najpierw.");
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        Logger::error("Initializacja CURL nie powiodla sie.");
        return false;
    }


    std::string url = "http://" + address + ":" + std::to_string(port) + "/logout";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers,  ("Token: " + token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        Logger::error("Blad podczas wykonywania zadania CURL: " + std::string(curl_easy_strerror(res)));
        curlCleanup(curl, headers);
        return false;
    }

    // Zakladamy, ze odpowiedz zawiera token w przypadku sukcesu
    if (response.empty()) {
        Logger::error("Pusta odpowiedz od serwera.");
        curlCleanup(curl, headers);
        return false;
    }

    try {
        json jsonResponse = json::parse(response);
        if(jsonResponse["resType"] == "ERROR") {
            Logger::error(jsonResponse["message"].get<std::string>());
            curlCleanup(curl, headers);
            return false;
        } else if (jsonResponse["resType"] == "MESSAGE") {
            Logger::info(jsonResponse["message"].get<std::string>());
            token.clear();
            curlCleanup(curl, headers);
            return true;
        } else {
            Logger::error("Serwerowi odwala");
        }
    } catch (const json::parse_error& e) {
        Logger::error("Blad parsowania odpowiedzi JSON: " + std::string(e.what()));
        curlCleanup(curl, headers);
        return false;
    }


    curlCleanup(curl, headers);

    return false;
}

bool ConnectionManager::changePassword(const std::string& password)  {
    if(token.empty()) {
        Logger::error("Brak tokenu sesji. Zaloguj sie najpierw.");
        return false;
    }

    if(password.empty()) {
        Logger::error("Haslo nie moze byc puste.");
        return false;
    }               

    CURL* curl = curl_easy_init();
    if (!curl) {
        Logger::error("Initializacja CURL nie powiodla sie.");
        return false;
    }

    json jsonBody;
    jsonBody["password"] = password;

    std::string body = jsonBody.dump();

    std::string url = "http://" + address + ":" + std::to_string(port) + "/changePassword";
    std::string response;
    std::vector<std::string> responseHeaders;


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);



    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers,  ("Token: " + token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        Logger::error("Blad podczas wykonywania zadania CURL: " + std::string(curl_easy_strerror(res)));
        curlCleanup(curl, nullptr);
        return false;
    }

    if(std::find(responseHeaders.begin(), responseHeaders.end(), "Server: STDB HTTP Server\r\n") == responseHeaders.end()) {
        Logger::error("Serwer jest nieprawidlowy.");
        curlCleanup(curl, nullptr);
        return false;
    }

    // Zakladamy, ze odpowiedz zawiera token w przypadku sukcesu
    if (response.empty()) {
        Logger::error("Pusta odpowiedz od serwera.");
        curlCleanup(curl, headers);
        return false;
    }
  
   try {
        json jsonResponse = json::parse(response);
        if(jsonResponse["resType"] == "ERROR") {
            Logger::error(jsonResponse["message"].get<std::string>());
            curlCleanup(curl, headers);
            return false;
        } else if (jsonResponse["resType"] == "MESSAGE") {
            Logger::info(jsonResponse["message"].get<std::string>());
            curlCleanup(curl, headers);
            return true;
        } else {
            Logger::error("Serwerowi odwala");
        }
    } catch (const json::parse_error& e) {
        Logger::error("Blad parsowania odpowiedzi JSON: " + std::string(e.what()));
        curlCleanup(curl, headers);
        return false;
    }

    curlCleanup(curl, headers);
    return false;
}