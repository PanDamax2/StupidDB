#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

#include "CommandParser.hpp"
#include "./database/Table.hpp"
#include "./database/Column.hpp"
#include "./database/Row.hpp"
#include "FileManager.hpp"
#include "Logger.hpp"


class QueryExecutor {
private:
    std::string databasePath;                              // Ścieżka do katalogu baz danych
    std::string currentDatabase;                           // Nazwa aktualnej bazy
    std::map<std::string, std::shared_ptr<Table>> tables;  // Cache otwartych tabel
    
public:
    // === KONSTRUKTOR ===
    explicit QueryExecutor(const std::string& databasePath = "db");
    
    //  Destruktor - 
    ~QueryExecutor();
    
    // === GŁÓWNA FUNKCJA WYKONUJĄCA ===
    // Wykonuje sparsowaną komendę
    bool execute(const ParsedCommand& cmd);
    
    // === GETTERY / SETTERY ===
    const std::string getCurrentDatabase() const { return currentDatabase; }
    const std::string getDatabasePath() const { return databasePath; }
    
private:
    // === DATABASE MANAGEMENT ===
    bool executeShowDatabases();
    bool executeUseDatabase(const ParsedCommand& cmd);
    bool executeDropDatabase(const ParsedCommand& cmd);
    bool executeChangeDatabaseName(const ParsedCommand& cmd);
    bool executeLogout();

    // === DDL (Data Definition Language) ===
    bool executeShowTables();
    bool executeCreateTable(const ParsedCommand& cmd);
    bool executeDropTable(const ParsedCommand& cmd);
    bool executeDescribeTable(const ParsedCommand& cmd);
    bool executeModifyTableName(const ParsedCommand& cmd);
    bool executeAddColumn(const ParsedCommand& cmd);
    bool executeModifyColumnName(const ParsedCommand& cmd);
    bool executeDropColumn(const ParsedCommand& cmd);

    // === DML (Data Manipulation Language) ===
    bool executeInsert(const ParsedCommand& cmd);
    bool executeSelectAll(const ParsedCommand& cmd);
    bool executeSelect(const ParsedCommand& cmd);
    bool executeUpdate(const ParsedCommand& cmd);
    bool executeDelete(const ParsedCommand& cmd);
    bool executeClearTable(const ParsedCommand& cmd);

    // === UTILITY ===
    void showHelp();
    
    // === METODY POMOCNICZE ===

    // Zwraca dokładną nazwę istniejącej bazy 
    std::string findExactDatabaseName(const std::string& dbName) const;
    
    // ZŁaduje tabelę do cache (jeśli jeszcze nie jest załadowana)
    std::shared_ptr<Table> getTable(const std::string& tableName);
    
    // Konwertuje string typu kolumny na ColumnType
    ColumnType parseColumnType(const std::string& typeStr, uint32_t& size);
    
    // Zwraca pełną ścieżkę do pliku tabeli
    std::string getTableFilePath(const std::string& tableName) const;
    
    // Zwraca ścieżkę do katalogu bieżącej bazy danych
    std::string getCurrentDatabasePath() const;
    
    // Sprawdza czy tabela istnieje w bieżącej bazie
    bool tableExists(const std::string& tableName) const;
    
    // Zapisuje wszystkie otwarte tabele
    void saveAllTables();
    void clearCache();
    
    // Znajduje wiersze spełniające warunki WHERE
    std::vector<int32_t> findMatchingRows(std::shared_ptr<Table> table, const std::map<std::string, std::string>& conditions);
};