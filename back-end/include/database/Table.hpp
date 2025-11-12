#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Column.hpp"
#include "Row.hpp"

// Maksymalne wartości
#define MAX_COLS_COUNT 32
#define MAX_ROWS_COUNT 1048576

// === KONSTRUKTOR HEADERA ===
struct TableHeader {
    uint8_t signature[4];      // 'STTB' - sygnatura pliku tabeli
    uint32_t colsCount;        // Liczba kolumn
    uint32_t rowsCount;        // Liczba wierszy
    uint32_t currentRowID;     // Następny wolny ID wiersza
    
    TableHeader();
};

class Table {
private:
    std::string tableName;                  // Nazwa tabeli
    std::string tableFile;                  // Ścieżka do pliku tabeli
    TableHeader header;                     // Nagłówek tabeli
    std::vector<Column> columns;            // Kolumny tabeli
    std::vector<Row> rows;                  // Wiersze w pamięci
    
    bool isModified;                        // Czy tabela została zmodyfikowana

public:
    // === KONSTRUKTORY / DESTRUKTOR ===
    
    //Konstruktor - tworzy nową tabelę
    Table(const std::string& tableName, const std::string& tableFile);
    // Destruktor - zapisuje zmiany jeśli potrzeba
    ~Table();
    
    // === GETTERY ===
    std::string getName()   const { return tableName; }
    std::string getFile()   const { return tableFile; }
    uint32_t getRowsCount() const { return header.rowsCount; }
    uint32_t getColsCount() const { return header.colsCount; }
    const std::vector<Column>& getColumns() const { return columns; }

    // === SETTERY ===
    void setName(const std::string& name);
    
    // === INICJALIZACJA ===
    //Odczytuje z pliku lub tworzy nową
    bool init();
    
    // === OPERACJE NA KOLUMNACH ===
    //Tworzy nową kolumnę w tabeli
    int createColumn(const Column& column);

    void renameColumn(int index, const std::string& newName); 
    void dropColumn(int index);                               
    void clearData();
    
    //Usuwa kolumnę z tabeli
    bool deleteColumn(const std::string& columnName);
    
    // Wyszukuje kolumnę po nazwie
    int findColumnByName(const std::string& name) const;
    
    // Pobiera kolumnę po indeksie
    const Column* getColumn(size_t index) const;
    
    // === OPERACJE NA WIERSZACH (CRUD) ===
    
    // INSERT - Wstawia nowy wiersz do tabeli
    int32_t insertRow(const Row& row);
    // INSERT - Wstawia wiersz z wektora wartości
    int32_t insertRow(const std::vector<CellValue>& values);
    // NSERT - Wstawia wiersz z wektora stringów (parsuje wartości)
    int32_t insertRowFromStrings(const std::vector<std::string>& values);
    // SELECT - Pobiera wszystkie wiersze
    std::vector<Row> selectAll() const;
    // SELECT - Pobiera wiersz po ID
    const Row* selectByID(int32_t rowID) const;
    // SELECT - Pobiera wybrane kolumny z wiersza
    Row selectColumns(int32_t rowID, const std::vector<std::string>& columnNames) const;
    // UPDATE - Aktualizuje wartość w wierszu
    bool updateCell(int32_t rowID, const std::string& columnName, const CellValue& newValue);
    // DELETE - Usuwa wiersz po ID
    bool deleteRow(int32_t rowID);
    
    // === SERIALIZACJA (zapis/odczyt do pliku) ===
    
    // Zapisuje strukturę tabeli do pliku (header + kolumny)
    bool writeStructure();
    // Odczytuje strukturę tabeli z pliku
    bool readStructure();
    // Zapisuje dane (wiersze) do pliku
    bool writeData();
    // Odczytuje dane (wiersze) z pliku
    bool readData();
    // Zapisuje całą tabelę (struktura + dane)
    bool save();
    // Odczytuje całą tabelę (struktura + dane)
    bool load();
    
    // === WYŚWIETLANIE ===
    
    // Wyświetla strukturę tabeli
    void printStructure() const;
    // Wyświetla zawartość tabeli
    void printData(size_t limit = 0) const;
    // Wyświetla statystyki tabeli
    void printStats() const;
    
private:
    // === METODY POMOCNICZE ===
    
    // Waliduje wiersz przed wstawieniem
    bool validateRow(const Row& row) const;
    // Generuje nowy unikalny ID wiersza
    int32_t generateRowID();
};