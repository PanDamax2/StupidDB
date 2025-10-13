#include <iostream>
#include <string>
#include <vector> 
#include <fstream>
#include <filesystem> 

#include "../headers/db.h"

using std::cout;       
using std::cin;        
using std::string;     
using std::vector;     
using std::endl;       
using std::getline;    
using std::ofstream;   
using std::ifstream;   
using std::stringstream;
namespace fs = std::filesystem;  
// using namespace std::filesystem;  

// Funkcja do wyświetlania pomocy
void show_help() {
    cout << "\n=== Dostepne komendy ===\n";
    cout << "- SHOW DATABASES; : Wyswietla dostepne bazy danych.\n";
    cout << "- SHOW TABLES; : Wyswietla dostepne tabele w biezacej bazie.\n";
    cout << "- USE <nazwa_bazy>; : Przelacza na baze danych (tworzy jesli nie istnieje).\n";
    cout << "- CREATETABLE('nazwa_tabeli') : Tworzy tabele w biezacej bazie.\n";
    cout << "- INSERT ('nazwa_tabeli') VALUES ('wartosc', 'wartosc2'); : Dodaje rekord.\n";
    cout << "- DELETEVALUES ('nazwa_tabeli') ('wartosc'); : Usuwa jeden rekord pasujacy do zapytania.\n";
    cout << "- SELECT ('nazwa_tabeli'); : Wyswietla wszystkie rekordy z tabeli z wartosciami.\n";
    cout << "- DELETETABLE('nazwa_tabeli'); : Usuwa tabele w biezacej bazie.\n";
    cout << "- help : Wyswietla te pomoc.\n";
    cout << "- exit : Zamyka program.\n";
}

void handle_command(const string& command) {
    if (command == "SHOW DATABASES;") {
        cout << "Wyswietlam liste baz danych...\n";
    }
    else if (command == "SHOW TABLES;") {
        cout << "Wyswietlam liste tabel w biezacej bazie...\n";
    }
    else if (command.rfind("USE ", 0) == 0) {
        cout << "Przelaczam na baze danych...\n";
    }
    else if (command.rfind("CREATETABLE('", 0) == 0) {
        cout << "Tworze tabele...\n";
    }
    else if (command.rfind("INSERT ('", 0) == 0) {
        cout << "Dodaje rekord do tabeli...\n";
    }
    else if (command.rfind("DELETEVALUES ('", 0) == 0) {
        cout << "Usuwam rekord z tabeli...\n";
    }
    else if (command.rfind("SELECT ('", 0) == 0) {
        cout << "Wyswietlam rekordy z tabeli...\n";
    }
    else if (command.rfind("DELETETABLE('", 0) == 0) {
        cout << "Usuwam tabele...\n";
    }
    else if (command == "help") {
        show_help();
    }
    else if (!command.empty()) {
        cout << "Nieznana komenda: " << command << "\n";
    }
}


int main() {
    const string DB_DIR = "db";
    DB db = DB();
    db.header.totalDataHeadersCount = 2137;
    db.writeStructure();



    if (!fs::exists(DB_DIR)) {
        fs::create_directory(DB_DIR);
        cout << "Utworzono folder glowny: " << DB_DIR << "\n";
    }
    
    cout << "=== StupidDB ODPALONA ===" << endl << endl;
    
    string command;
    while (true) {
        cout << "> ";
        getline(cin, command);

        if (command == "exit") {
            cout << "Zamykanie StupidDB..." << endl;
            break;
        }

        
        
        handle_command(command);

        //cout << "Nieznana komenda: " << command << "\n";
    }
    return 0;
