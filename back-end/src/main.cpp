#include <iostream>
#include <string>

#include "../include/Logger.hpp"
#include "../include/FileManager.hpp"
#include "../test/Test.hpp"
#include "../include/database/Table.hpp"      
#include "../include/database/Column.hpp"     
#include "../include/database/Row.hpp"   
#include "../include/CommandParser.hpp"
#include "../include/QueryExecutor.hpp"     



int main() {
    // Start log
    Logger::info("StupidDB uruchomiony");
    showWelcomeBanner();

    // -------------------------------------------

    // blackout zwiększyć essssss

    // showHelp();

    


    // -------------------------------------------

    
    // testFileManager();
    // testLogger();
    


    // Główna pętla
    // std::string input;
    // while (true) {
    //     std::cout << "> ";
    //     std::getline(std::cin, input);

    //     // Pomijamy puste linie i linie z samymi spacjami i tabami
    //     if (input.empty() || input.find_first_not_of(" \t\n\r") == std::string::npos) continue;
    //     // Obsługa komend postawowych
    //     if (input == "exit")  { Logger::info("Zamykanie StupidDB"); break; }
    //     if (input == "help")  { showHelp();           continue; }
    //     if (input == "clear") { system("cls||clear"); continue; }

       
    //     Logger::warn("Nieznana komenda: " + input);
    //     std::cout << "Wpisz 'help' aby zobaczyc dostepne komendy.\n";
    // }

    QueryExecutor executor;
    CommandParser parser;

    std::string input;
    while (true) {
        std::cout << (executor.getCurrentDatabase().empty() ? "stupiddb" : executor.getCurrentDatabase()) << "> ";
        std::cout.flush();

        if (!std::getline(std::cin, input)) {
            std::cout << "\nDo widzenia!\n";
            break;
        }

        if (input.empty()) continue;

        ParsedCommand cmd;
        try {
            cmd = parser.parse(input);
        } catch (const std::exception& e) {
            std::cout << "Bład: " << e.what() << "\n\n";
            continue;
        }

        if (cmd.type == CommandType::EXIT) {
            std::cout << "Zamykanie...\n";
            break;
        }

        bool success = executor.execute(cmd);
        if (!success && cmd.type != CommandType::HELP && cmd.type != CommandType::UNKNOWN) {
            std::cout << "Operacja nie powiodla się.\n\n";
        }
    }
   


    return 0;
}