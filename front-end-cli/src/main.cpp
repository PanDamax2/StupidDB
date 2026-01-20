#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
#include <atomic>
#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
#endif

#include "../include/Logger.hpp"
#include "../include/ConnectionManager.hpp"

std::atomic<bool> keepRunning{true};

void signalHandler(int sig) {
    if(sig == SIGINT || sig == SIGTERM)
        keepRunning = false;
}

std::string getPasswordInput() {
    std::string password;
    #ifdef _WIN32
        char ch;
        while ((ch = _getch()) != '\r' && ch != '\n') { // Enter key
            if (ch == '\b') { // Backspace key
                if (!password.empty()) {
                    password.pop_back();
                }
            } else {
                password.push_back(ch);
            }
        }
        
    #else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        std::getline(std::cin, password);
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif
    std::cout << std::endl;
    return password;
}

int main(int argc, char **argv) {   

    std::string host = "localhost";
    int port = 8080;
    bool changePassword = false;

    char* prevArg = nullptr;
    for(int i = 1; i < argc; i++) {
        if(prevArg != nullptr) {
            if(strcmp(prevArg, "--host") == 0 || strcmp(prevArg, "-h") == 0) {
                host = argv[i];
            } else if(strcmp(prevArg, "--port")  == 0 || strcmp(prevArg, "-p") == 0) {
                try {
                    port = std::stoi(argv[i]);
                } catch(...) {
                    Logger::error("Nieprawidlowy numer portu: " + std::string(argv[i]));
                    return -1;
                }
            }
        }
        if(strcmp(argv[i], "--change-password") == 0 || strcmp(argv[i], "-cp") == 0) {
            changePassword = true;
        }

        prevArg = argv[i];
    }


    showWelcomeBanner();
    Logger::info("Host: " + host + "\nPort: " + std::to_string(port));

    if(changePassword) {
        std::cout << "Podaj aktualne haslo: " << std::endl;
        std::string currentPassword = getPasswordInput();
        std::cout << "Podaj nowe haslo: " << std::endl;
        std::string newPassword = getPasswordInput();

        Logger::info("Host: " + host + "\nPort: " + std::to_string(port));
        ConnectionManager connMgr(host, port);
        if (!connMgr.login(currentPassword)) {
            return -1;
        }
        connMgr.changePassword(newPassword); 
        connMgr.logout();
        return 0;
    }

    std::cout << "Podaj haslo do bazy danych: " << std::endl;
    std::string password = getPasswordInput();


    ConnectionManager connMgr(host, port);
    if (!connMgr.login(password)) {
        return -1;
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "Wpisz help po pomoc lub exit aby wyjsc" << std::endl;
    while(keepRunning) {
        std::cout << ">" << std::flush;
        std::string query;
        std::getline(std::cin, query);
        
        if (query == "exit" || query == "EXIT") {
            break;
        }
        
        connMgr.query(query);
    }

    connMgr.logout();
    return 0;
}  