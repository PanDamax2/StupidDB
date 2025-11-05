#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

enum class CommandType {
    UNKNOWN,
    
    // === Database Management ===
    SHOW_DATABASES,
    USE_DATABASE,
    DROP_DATABASE,
    CHANGE_DATABASE_NAME,
    
    // === DDL (Data Definition Language) ===
    SHOW_TABLES,
    CREATE_TABLE,
    DROP_TABLE,
    DESCRIBE_TABLE,
    MODIFY_TABLE_NAME,
    ADD_COLUMN,
    MODIFY_COLUMN_NAME,
    DROP_COLUMN,
    
    // === DML (Data Manipulation Language) ===
    INSERT,
    SELECT_ALL,
    SELECT,
    UPDATE,
    DELETE,
    CLEAR_TABLE,
    
    // === Utility ===
    HELP,
    EXIT
};

/**
 * ParsedCommand - Reprezentuje sparsowaną komendę
 */
struct ParsedCommand {
    CommandType type;
    std::string tableName;
    std::string databaseName;
    std::string oldName;                        // Dla RENAME operacji
    std::string newName;                        // Dla RENAME operacji
    std::string columnName;                     // Dla operacji na kolumnach
    std::string columnType;                     // Typ kolumny
    std::string username;                       // Dla zarządzania użytkownikami
    std::string password;                       // Dla zarządzania użytkownikami
    std::vector<std::string> columns;           // Lista kolumn
    std::vector<std::string> values;            // Wartości do INSERT/UPDATE
    std::vector<std::string> columnTypes;       // Typy kolumn dla CREATE TABLE
    std::map<std::string, std::string> conditions; // WHERE conditions
    
    ParsedCommand() : type(CommandType::UNKNOWN) {}
};

/**
 * CommandParser - Parser komend SQL
 */
class CommandParser {
public:
    static ParsedCommand parse(const std::string& input);
    static bool isValidCommand(const std::string& input);
    
private:
    // Metody pomocnicze
    static std::string trim(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::string extractBetweenParentheses(const std::string& str);
    static std::map<std::string, std::string> parseWhereClause(const std::string& whereClause);
    
    // Parsery dla konkretnych komend - Database Management
    static ParsedCommand parseShowDatabases(const std::string& input);
    static ParsedCommand parseUse(const std::string& input);
    static ParsedCommand parseDropDatabase(const std::string& input);
    static ParsedCommand parseChangeDbName(const std::string& input);
    
    // Parsery DDL
    static ParsedCommand parseShowTables(const std::string& input);
    static ParsedCommand parseCreateTable(const std::string& input);
    static ParsedCommand parseDropTable(const std::string& input);
    static ParsedCommand parseDescribe(const std::string& input);
    static ParsedCommand parseModifyTableName(const std::string& input);
    static ParsedCommand parseAddColumn(const std::string& input);
    static ParsedCommand parseModifyColumnName(const std::string& input);
    static ParsedCommand parseDropColumn(const std::string& input);
    
    // Parsery DML
    static ParsedCommand parseInsert(const std::string& input);
    static ParsedCommand parseSelectAll(const std::string& input);
    static ParsedCommand parseSelect(const std::string& input);
    static ParsedCommand parseUpdate(const std::string& input);
    static ParsedCommand parseDelete(const std::string& input);
    static ParsedCommand parseClearTable(const std::string& input);
    
    // Parsery User Management
    static ParsedCommand parseChangeUsername(const std::string& input);
    static ParsedCommand parseChangePassword(const std::string& input);
    static ParsedCommand parseAddUser(const std::string& input);
    static ParsedCommand parseRemoveUser(const std::string& input);
    static ParsedCommand parseShowUsers(const std::string& input);
};