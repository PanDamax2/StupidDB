#include <iostream>
#include "../include/Logger.hpp"

// Kod na windows przechwytuje kolor cmd
#ifdef _WIN32
    #include <windows.h>
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    static WORD originalColor = 7; 

    struct InitColor {
        InitColor() {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
                originalColor = csbi.wAttributes;
            }
        }
    };
    static InitColor init;
#endif

// Wyświetla banner powitalny 
void showWelcomeBanner() {
    std::cout << "\n";
    std::cout << "  ####### ########  ##     ## ######  ## ######    ######    ###### \n";
    std::cout << "  ##         ##     ##     ## ##   ## ## ##    ##  ##    ##  ##   ##     \n";
    std::cout << "  ######     ##     ##     ## ######  ## ##     ## ##     ## #####        \n";
    std::cout << "      ##     ##     ##     ## ##      ## ##    ##  ##    ##  ##   ##     \n";
    std::cout << "  #######    ##      #######  ##      ## ######    ######    ######    \n";

    std::cout << "\n";
}

// Wyświetlania menun pomocy
void showHelp() {
    std::cout << "\n";
    std::cout << "===============================================================\n";
    std::cout << "                     StupidDB - Pomoc                          \n";
    std::cout << "===============================================================\n";
    std::cout << "\n";
    
    std::cout << "=== Komendy systemowe ===\n";
    std::cout << "  help                    - Wyswietla te pomoc\n";
    std::cout << "  clear / cls             - Czysci ekran\n";
    std::cout << "  exit / quit             - Zamyka program\n";
    std::cout << "\n";
    
    std::cout << "=== Zarzadzanie bazami danych ===\n";
    std::cout << "  SHOW DATABASES;         - Wyswietla wszystkie bazy danych\n";
    std::cout << "  USE nazwa_bazy;         - Przelacza na baze danych (tworzy jesli nie istnieje)\n";
    std::cout << "\n";
    
    std::cout << "=== Zarzadzanie tabelami (DDL) ===\n";
    std::cout << "  SHOW TABLES;            - Wyswietla tabele w biezacej bazie\n";
    std::cout << "  DESCRIBE tabela;        - Wyswietla strukture tabeli\n";
    std::cout << "  DESC tabela;            - Skrot od DESCRIBE\n";
    std::cout << "\n";
    std::cout << "  CREATE TABLE tabela (   - Tworzy nowa tabele\n";
    std::cout << "    col1 INT,\n";
    std::cout << "    col2 VARCHAR(50),\n";
    std::cout << "    col3 FLOAT,\n";
    std::cout << "    col4 BOOL\n";
    std::cout << "  );\n";
    std::cout << "\n";
    std::cout << "  DROP TABLE tabela;      - Usuwa tabele\n";
    std::cout << "\n";
    
    std::cout << "=== Operacje na danych (DML) ===\n";
    std::cout << "  INSERT INTO tabela VALUES (val1, val2, ...);\n";
    std::cout << "                          - Wstawia nowy wiersz\n";
    std::cout << "\n";
    std::cout << "  SELECT * FROM tabela;   - Wyswietla wszystkie dane\n";
    std::cout << "  SELECT * FROM tabela WHERE col=val;\n";
    std::cout << "                          - Wyswietla wiersze spelniajace warunek\n";
    std::cout << "\n";
    std::cout << "  UPDATE tabela SET col=val WHERE col2=val2;\n";
    std::cout << "                          - Aktualizuje dane\n";
    std::cout << "\n";
    std::cout << "  DELETE FROM tabela WHERE col=val;\n";
    std::cout << "                          - Usuwa wiersze\n";
    std::cout << "\n";
    
    std::cout << "=== Typy danych ===\n";
    std::cout << "  INT                     - Liczba calkowita (32-bit)\n";
    std::cout << "  FLOAT                   - Liczba zmiennoprzecinkowa\n";
    std::cout << "  VARCHAR(n)              - Tekst o maksymalnej dlugosci n\n";
    std::cout << "  BOOL                    - Wartosc logiczna (true/false)\n";
    std::cout << "\n";
    
    std::cout << "=== Przyklady uzycia ===\n";
    std::cout << "  USE my_database;\n";
    std::cout << "  CREATE TABLE users (id INT, name VARCHAR(50), age INT);\n";
    std::cout << "  INSERT INTO users VALUES (1, 'Jan', 25);\n";
    std::cout << "  SELECT * FROM users;\n";
    std::cout << "  UPDATE users SET age=26 WHERE id=1;\n";
    std::cout << "  DELETE FROM users WHERE id=1;\n";
    std::cout << "\n";
    
    std::cout << "Wskazowka: Wszystkie komendy SQL powinny konczyc sie srednikiem (;)\n";
    std::cout << "\n";
}


// Loguje wiadomość z prefiksem poziomu i kolorem
void Logger::log(LogLevel level, const std::string& message) {
    
    const char* prefix;
#ifdef _WIN32
    WORD color = 7;
#endif
    if (level == LogLevel::Info) {
        prefix = "[INFO]";
#ifdef _WIN32
        color = 10;  // zielony
#endif
    }
    else if (level == LogLevel::Warning) {
        prefix = "[WARNING]";
#ifdef _WIN32
        color = 14;  // żółty
#endif
    }
    else if (level == LogLevel::Error) {
        prefix = "[ERROR]";
#ifdef _WIN32
        color = 12;  // czerwony
#endif
    }
    else {
        prefix = "[UNKNOWN]";
#ifdef _WIN32
        color = 7;
#endif
    }
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, color);
    std::cout << prefix << ": " << message << "\n";
    SetConsoleTextAttribute(hConsole, originalColor);
#else
    const char* ansi = "\033[0m";
    if      (level == LogLevel::Info)       ansi = "\033[32m";      // zielony
    else if (level == LogLevel::Warning)    ansi = "\033[33m";      // żółty
    else if (level == LogLevel::Error)      ansi = "\033[31m";      // czerwony

    std::cout << ansi << prefix << ": " << message << "\033[0m\n";
#endif
}

// Skróty
void Logger::info(const std::string& msg)  { log(LogLevel::Info, msg); }
void Logger::warn(const std::string& msg)  { log(LogLevel::Warning, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::Error, msg); }