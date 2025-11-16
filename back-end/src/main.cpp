#include <string>
#include <filesystem>
#include <cstring>

#include "../include/HTTP_Server.hpp"   
#include "../include/Logger.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;

    // Argument parsing
    if(argc > 1) {
        char *prevValue = nullptr;
        for(int i = 1; i < argc; i++) {
            if(prevValue && strcmp(prevValue, "--path") == 0) {
                std::string path = std::string(argv[i]);
                if(fs::exists(path)) {
                    fs::current_path(path);
                } else {
                    Logger::error("Podany katalog nie istnieje");
                    return -1;
                }
            }

            if(prevValue && strcmp(prevValue, "--port") == 0) {
                try {
                    port = std::stoi(argv[i]);
                } catch(...) {
                    Logger::error("Port musi być liczbą całkowitą");
                    return -1;
                }
            }

            prevValue = argv[i];
        }
    }    

    // Start log
    Logger::info("StupidDB uruchomiony");
    showWelcomeBanner();

    HTTP_Server server(port);
    server.start();

    return 0;
}