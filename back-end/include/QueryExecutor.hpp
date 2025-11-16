#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <mutex>
#include <shared_mutex>

#include "CommandParser.hpp"
#include "./database/Table.hpp"
#include "./database/Column.hpp"
#include "./database/Row.hpp"
#include "FileManager.hpp"
#include "Logger.hpp"
#include "HTTP_Status.hpp"

#include "../libs/json.hpp"

using json = nlohmann::json;

enum class HTTP_Status;

enum class QueryType {
    ERROR_,
    MESSAGE_,
    TABLE_
};

class QueryResponse {
private:
    json rowsToJSON();
public:
    HTTP_Status status;
    QueryType type;
    std::string message;
    std::vector<std::string> cols;
    std::vector<RowValues> rows;

    std::string toJSON();
    // Helpery
    static QueryResponse genError(HTTP_Status status, std::string message);
    static QueryResponse genMessage(std::string message);
    static QueryResponse genTable(std::vector<std::string> cols, std::vector<RowValues> rows);

};

class QueryExecutor {
private:
    std::string databasePath;                              // Ścieżka do katalogu baz danych
    std::string currentDatabase;                           // Nazwa aktualnej bazy
    std::map<std::string, std::shared_ptr<Table>> tables;  // Cache otwartych tabel
    static std::shared_mutex m;                    // Mutex do synchronizacji dostępu   
public:
    // === KONSTRUKTOR ===
    explicit QueryExecutor(const std::string& databasePath = "db");
    
    //  Destruktor - 
    ~QueryExecutor();
    
    // === GŁÓWNA FUNKCJA WYKONUJĄCA ===
    // Wykonuje sparsowaną komendę
    QueryResponse execute(const ParsedCommand& cmd);
    
    // === GETTERY / SETTERY ===
    const std::string getCurrentDatabase() const { return currentDatabase; }
    const std::string getDatabasePath() const { return databasePath; }
    
private:
    // === DATABASE MANAGEMENT ===
    QueryResponse executeShowDatabases();
    QueryResponse executeUseDatabase(const ParsedCommand& cmd);
    QueryResponse executeDropDatabase(const ParsedCommand& cmd);
    QueryResponse executeUseNone();

    // === DDL (Data Definition Language) ===
    QueryResponse executeShowTables();
    QueryResponse executeCreateTable(const ParsedCommand& cmd);
    QueryResponse executeDropTable(const ParsedCommand& cmd);
    QueryResponse executeDescribeTable(const ParsedCommand& cmd);
    QueryResponse executeModifyTableName(const ParsedCommand& cmd);
    QueryResponse executeAddColumn(const ParsedCommand& cmd);
    QueryResponse executeModifyColumnName(const ParsedCommand& cmd);
    QueryResponse executeDropColumn(const ParsedCommand& cmd);

    // === DML (Data Manipulation Language) ===
    QueryResponse executeInsert(const ParsedCommand& cmd);
    QueryResponse executeSelectAll(const ParsedCommand& cmd);
    QueryResponse executeSelect(const ParsedCommand& cmd);
    QueryResponse executeUpdate(const ParsedCommand& cmd);
    QueryResponse executeDelete(const ParsedCommand& cmd);
    QueryResponse executeClearTable(const ParsedCommand& cmd);

    // === UTILITY ===
    std::string getHelp();
    
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