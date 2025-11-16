#include "../../include/TCP_Server.hpp"
#include "../../include/Logger.hpp"
#include <vector>
#include <thread>

#ifdef _WIN32
    #define close closesocket
#else
    #define INVALID_SOCKET 0
    #define SOCKET_ERROR -1
#endif

TCP_Server::TCP_Server(int port, std::function<void(socket_t client_fd)> handler){
    if(port <= 0 || port > 65535) {
        Logger::warn("Nieprawidlowy port podany. Uzywanie domyslnego portu " + std::to_string(DEFAULT_PORT) + ".");
        this->port = DEFAULT_PORT;
    } else {
        this->port = port;
    }

    if(!handler) {
        Logger::error("Brak funkcji obslugi zapytan. Serwer nie moze zostac uruchomiony.");
    } else {
        this->requestHandler = handler;
    }
}

void TCP_Server::start() {
    #ifdef _WIN32
        WSADATA wsaData;
        int wsaInit = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (wsaInit != 0) {
            Logger::error("Inicjalizacja Winsock nie powiodla sie.");
            return;
        }
    #endif
    
    socket_t server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Tworzenie gniazda (IPv4, TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        Logger::error("Nie mozna utworzyc gniazda TCP.");
        #ifdef _WIN32
            WSACleanup();
        #endif
        return;
    }

    // Ustawienie opcji gniazda (np. ponowne użycie portu)
    #ifdef _WIN32
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))) {
    #else
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    #endif
        Logger::error("Nie mozna ustawic opcji gniazda.");
        #ifdef _WIN32
            WSACleanup();
        #endif
        close(server_fd);
        return;
    }

    // Konfiguracja adresu serwera
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // nasłuchuj na wszystkich interfejsach
    address.sin_port = htons(port);

    // Przypisanie gniazda do adresu i portu
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        Logger::error("Nie mozna powiazac gniazda z adresem.");
        close(server_fd);
        return;
    }

    // Nasłuchiwanie na połączenia
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        Logger::error("Nie mozna nasluchiwac na gniezdzie.");
        #ifdef _WIN32
            WSACleanup();
        #endif
        close(server_fd);
        return;
    }

    Logger::info("Serwer TCP nasluchuje na porcie " + std::to_string(port) + "...");
    std::vector<std::thread> threads;
    while (true) {
        // Oczekiwanie na połączenie klienta
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            Logger::error("Nie mozna zaakceptowac polaczenia klienta.");
            #ifdef _WIN32
                WSACleanup();
            #endif
            continue;
        }

        // Obsługa klienta w osobnym wątku
        threads.push_back(std::thread(&TCP_Server::handleRequest, this, client_fd, address));

        for (auto it = threads.begin(); it != threads.end(); ) {
            if (it->joinable()) {
                it++;
            } else {
                it = threads.erase(it);
            }
        }
    }

    close(server_fd);
    #ifdef _WIN32
        WSACleanup();
    #endif
}

void TCP_Server::handleRequest(socket_t client_fd, sockaddr_in address) {
    Logger::info("Polaczono z klientem: " + std::string(inet_ntoa(address.sin_addr)) +
                 ":" + std::to_string(ntohs(address.sin_port)));

    // Obsługa klienta
    requestHandler(client_fd);

    Logger::info("Rozlaczono z klientem: " + std::string(inet_ntoa(address.sin_addr)) +
                ":" + std::to_string(ntohs(address.sin_port)));

    #ifdef _WIN32
        shutdown(client_fd, SD_BOTH);
    #endif
    close(client_fd);
}