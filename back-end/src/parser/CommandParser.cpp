#include "../../include/CommandParser.hpp"
#include "../../include/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

// === GŁÓWNA FUNKCJA PARSUJĄCA ===
ParsedCommand CommandParser::parse(const std::string& input) {
    std::string trimmedInput = trim(input);

    // Usuwamy średnik
    if (!trimmedInput.empty() && trimmedInput.back() == ';') {
        trimmedInput.pop_back();
        trimmedInput = trim(trimmedInput);
    }

    if (trimmedInput.empty()) {
        return ParsedCommand();
    }

    std::string upper = toUpper(trimmedInput);

    // === DATABASE MANAGEMENT ===
    if (upper == "SHOWDATABASES" || upper == "SHOW DATABASES") {
        return parseShowDatabases(trimmedInput);
    }
    if (startsWith(upper, "USE ")) {
        return parseUse(trimmedInput);
    }
    if (startsWith(upper, "DROPDATABASE ") || startsWith(upper, "DROP DATABASE ")) {
        return parseDropDatabase(trimmedInput);
    }
    if (startsWith(upper, "CHANGEDBNAME ")) {
        return parseChangeDbName(trimmedInput);
    }

    // === DDL ===
    if (upper == "SHOWTABLES" || upper == "SHOW TABLES") {
        return parseShowTables(trimmedInput);
    }
    if (startsWith(upper, "CREATETABLE ") || startsWith(upper, "CREATE TABLE ")) {
        return parseCreateTable(trimmedInput);
    }
    if (startsWith(upper, "DROPTABLE ") || startsWith(upper, "DROP TABLE ")) {
        return parseDropTable(trimmedInput);
    }
    if (startsWith(upper, "DESCRIBE ") || startsWith(upper, "DESC ")) {
        return parseDescribe(trimmedInput);
    }
    if (startsWith(upper, "MODIFYTABLE ")) {
        return parseModifyTableName(trimmedInput);
    }
    if (startsWith(upper, "ADDCOLUMN ")) {
        return parseAddColumn(trimmedInput);
    }
    if (startsWith(upper, "MODIFYCOLUMN ")) {
        return parseModifyColumnName(trimmedInput);
    }
    if (startsWith(upper, "DROPCOLUMN ")) {
        return parseDropColumn(trimmedInput);
    }

    // === DML ===
    if (startsWith(upper, "INSERTINTO ") || startsWith(upper, "INSERT INTO ")) {
        return parseInsert(trimmedInput);
    }
    if (startsWith(upper, "SELECTALL ")) {
        return parseSelectAll(trimmedInput);
    }
    if (startsWith(upper, "SELECT ")) {
        return parseSelect(trimmedInput);
    }
    if (startsWith(upper, "UPDATE ")) {
        return parseUpdate(trimmedInput);
    }
    if (startsWith(upper, "DELETE ")) {
        return parseDelete(trimmedInput);
    }
    if (startsWith(upper, "CLEARTABLE ")) {
        return parseClearTable(trimmedInput);
    }

    // === UTILITY ===
    if (upper == "HELP") {
        ParsedCommand cmd;
        cmd.type = CommandType::HELP;
        return cmd;
    }
    if (upper == "EXIT" || upper == "QUIT") {
        ParsedCommand cmd;
        cmd.type = CommandType::EXIT;
        return cmd;
    }

    if (upper == "LOGOUT") {
        ParsedCommand cmd;
        cmd.type = CommandType::LOGOUT;
        return cmd;
    }

    #ifdef TEST
    Logger::warn("Parser: Nierozpoznana komenda: " + trimmedInput);
    #endif
    return ParsedCommand();
}

// === DATABASE MANAGEMENT ===
ParsedCommand CommandParser::parseShowDatabases(const std::string&) {
    ParsedCommand cmd;
    cmd.type = CommandType::SHOW_DATABASES;
    return cmd;
}

ParsedCommand CommandParser::parseUse(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::USE_DATABASE;

    size_t start = input.find(' ') + 1;
    if (start == std::string::npos) {
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }
    cmd.databaseName = trim(input.substr(start));
    return cmd;
}

ParsedCommand CommandParser::parseDropDatabase(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::DROP_DATABASE;

    std::string upper = toUpper(input);
    size_t pos = upper.find("DATABASE");
    if (pos != std::string::npos) pos += 8;
    else pos = input.find(' ') + 1;

    cmd.databaseName = trim(input.substr(pos));
    return cmd;
}

ParsedCommand CommandParser::parseChangeDbName(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::CHANGE_DATABASE_NAME;

    size_t start = input.find(' ') + 1;
    std::string rest = trim(input.substr(start));
    std::vector<std::string> parts = split(rest, ' ');

    if (parts.size() >= 2) {
        cmd.oldName = parts[0];
        cmd.newName = parts[1];
    } else {
        cmd.type = CommandType::UNKNOWN;
    }
    return cmd;
}

// === DDL ===
ParsedCommand CommandParser::parseShowTables(const std::string&) {
    ParsedCommand cmd;
    cmd.type = CommandType::SHOW_TABLES;
    return cmd;
}

ParsedCommand CommandParser::parseCreateTable(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::CREATE_TABLE;

    size_t tablePos = toUpper(input).find("TABLE") + 5;
    size_t openPos = input.find('(', tablePos);
    if (openPos == std::string::npos) {
        cmd.tableName = trim(input.substr(tablePos));
        return cmd;
    }

    cmd.tableName = trim(input.substr(tablePos, openPos - tablePos));
    std::string defs = extractBetweenParentheses(input);

    std::vector<std::string> colDefs = split(defs, ',');
    for (std::string def : colDefs) {
        def = trim(def);
        size_t space = def.find(' ');
        if (space == std::string::npos) continue;

        std::string name = trim(def.substr(0, space));
        std::string type = trim(def.substr(space + 1));

        cmd.columns.push_back(name);
        cmd.columnTypes.push_back(type);
    }

    return cmd;
}

ParsedCommand CommandParser::parseDropTable(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::DROP_TABLE;

    size_t pos = toUpper(input).find("TABLE") + 5;
    cmd.tableName = trim(input.substr(pos));
    return cmd;
}

ParsedCommand CommandParser::parseDescribe(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::DESCRIBE_TABLE;

    size_t pos = input.find(' ') + 1;
    cmd.tableName = trim(input.substr(pos));
    return cmd;
}

ParsedCommand CommandParser::parseModifyTableName(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::MODIFY_TABLE_NAME;

    size_t start = input.find(' ') + 1;
    std::string rest = trim(input.substr(start));
    std::vector<std::string> parts = split(rest, ' ');

    if (parts.size() >= 2) {
        cmd.oldName = parts[0];
        cmd.newName = parts[1];
    } else {
        cmd.type = CommandType::UNKNOWN;
    }
    return cmd;
}

ParsedCommand CommandParser::parseAddColumn(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::ADD_COLUMN;

    size_t start = input.find(' ') + 1;
    std::string rest = trim(input.substr(start));
    std::vector<std::string> parts = split(rest, ' ');

    if (parts.size() >= 3) {
        cmd.tableName = parts[0];
        cmd.columnName = parts[1];
        cmd.columnType = parts[2];
    } else {
        cmd.type = CommandType::UNKNOWN;
    }
    return cmd;
}

ParsedCommand CommandParser::parseModifyColumnName(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::MODIFY_COLUMN_NAME;

    size_t start = input.find(' ') + 1;
    std::string rest = trim(input.substr(start));
    std::vector<std::string> parts = split(rest, ' ');

    if (parts.size() >= 3) {
        cmd.tableName = parts[0];
        cmd.oldName = parts[1];
        cmd.newName = parts[2];
    } else {
        cmd.type = CommandType::UNKNOWN;
    }
    return cmd;
}

ParsedCommand CommandParser::parseDropColumn(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::DROP_COLUMN;

    size_t start = input.find(' ') + 1;
    std::string rest = trim(input.substr(start));
    std::vector<std::string> parts = split(rest, ' ');

    if (parts.size() >= 2) {
        cmd.tableName = parts[0];
        cmd.columnName = parts[1];
    } else {
        cmd.type = CommandType::UNKNOWN;
    }
    return cmd;
}

// === DML ===
ParsedCommand CommandParser::parseInsert(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::INSERT;

    // Znajdź "VALUES"
    size_t valuesPos = toUpper(input).find("VALUES");
    if (valuesPos == std::string::npos) {
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    // Znajdź początek nawiasu po "VALUES"
    size_t openParenPos = input.find('(', valuesPos);
    if (openParenPos == std::string::npos) {
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    // Znajdź koniec nawiasu
    size_t closeParenPos = input.rfind(')');
    if (closeParenPos == std::string::npos || closeParenPos < openParenPos) {
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    // Wyodrębnij zawartość nawiasu
    std::string valuesStr = input.substr(openParenPos + 1, closeParenPos - openParenPos - 1);

    // Znajdź nazwę tabeli (między INTO a VALUES)
    size_t tableStart = toUpper(input).find("INTO") + 4;
    size_t tableEnd = toUpper(input).find("VALUES", tableStart);
    if (tableEnd == std::string::npos) tableEnd = openParenPos;
    cmd.tableName = trim(input.substr(tableStart, tableEnd - tableStart));

    // Rozdziel wartości po przecinkach
    cmd.values = split(valuesStr, ',');

    // Usuń cudzysłowy i spacje
    for (std::string& v : cmd.values) {
        v = trim(v);
        if (!v.empty() && (v.front() == '\'' || v.front() == '"')) {
            if (v.back() == v.front()) {
                v = v.substr(1, v.size() - 2);
            }
        }
    }

    return cmd;
}

ParsedCommand CommandParser::parseSelectAll(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::SELECT_ALL;

    size_t pos = input.find(' ') + 1;
    cmd.tableName = trim(input.substr(pos));
    return cmd;
}

ParsedCommand CommandParser::parseSelect(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::SELECT;

    std::string upper = toUpper(input);
    size_t wherePos = upper.find("WHERE");

    size_t selectPos = upper.find("SELECT");
    if (selectPos == std::string::npos) {
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    std::string afterSelect = trim(input.substr(selectPos + 6));

    if (wherePos != std::string::npos) {
        cmd.tableName = trim(input.substr(selectPos + 6, wherePos - (selectPos + 6)));
        std::string where = trim(input.substr(wherePos + 5));

        if (where.empty()) {
            Logger::warn("Niepoprawna skladnia: brak warunku po WHERE.");
            cmd.type = CommandType::UNKNOWN;
            return cmd;
        }

        cmd.conditions = parseWhereClause(where);
    } else {
        if (afterSelect.find('=') != std::string::npos) {
            Logger::warn("Niepoprawna skladnia: brak slowa WHERE przed warunkiem.");
            cmd.type = CommandType::UNKNOWN;
            return cmd;
        }

        cmd.tableName = trim(afterSelect);
    }

    if (cmd.tableName.empty()) {
        Logger::warn("Niepoprawna skladnia: brak nazwy tabeli po SELECT.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    cmd.columns.clear(); 
    return cmd;
}

ParsedCommand CommandParser::parseUpdate(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::UPDATE;

    std::string upper = toUpper(input);
    size_t setPos = upper.find("SET");
    if (setPos == std::string::npos) {
        Logger::warn("Niepoprawna skladnia: brak słowa SET.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    cmd.tableName = trim(input.substr(6, setPos - 6));
    if (cmd.tableName.empty()) {
        Logger::warn("Niepoprawna skladnia: brak nazwy tabeli po UPDATE.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    size_t wherePos = upper.find("WHERE", setPos);
    std::string setPart = wherePos == std::string::npos ?
        trim(input.substr(setPos + 3)) :
        trim(input.substr(setPos + 3, wherePos - setPos - 3));

    size_t eq = setPart.find('=');
    if (eq == std::string::npos) {
        Logger::warn("Niepoprawna skladnia: brak operatora '=' w części SET.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    
    cmd.columns.push_back(trim(setPart.substr(0, eq)));
    std::string val = trim(setPart.substr(eq + 1));
    if (!val.empty() && (val.front() == '\'' || val.front() == '"')) {
        val = val.substr(1, val.size() - 2);
    }
    cmd.values.push_back(val);
    

    if (wherePos != std::string::npos) {
        std::string whereClause = trim(input.substr(wherePos + 5));
        if (whereClause.empty()) {
            Logger::warn("Niepoprawna skladnia: brak warunku po WHERE.");
            cmd.type = CommandType::UNKNOWN;
            return cmd;
        }

        cmd.conditions = parseWhereClause(trim(input.substr(wherePos + 5)));
    } else{
        if (setPart.find('=') != std::string::npos && upper.find("WHERE") == std::string::npos) {
            Logger::warn("UPDATE wymaga WHERE.");
            cmd.type = CommandType::UNKNOWN;
            return cmd;
        }
    }

    return cmd;
}

ParsedCommand CommandParser::parseDelete(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::DELETE;

    std::string upper = toUpper(input);
    size_t wherePos = upper.find("WHERE");

    std::string afterDelete = trim(input.substr(6));

    if (wherePos == std::string::npos) {
        Logger::warn("DELETE wymaga WHERE.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    cmd.tableName = trim(input.substr(6, wherePos - 6));
    if (cmd.tableName.empty()) {
        Logger::warn("Niepoprawna skladnia: brak nazwy tabeli po DELETE.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    std::string whereClause = trim(input.substr(wherePos + 5));
    if (whereClause.empty()) {
        Logger::warn("Niepoprawna skladnia: brak warunku po WHERE.");
        cmd.type = CommandType::UNKNOWN;
        return cmd;
    }

    cmd.conditions = parseWhereClause(whereClause);
    return cmd;
}

ParsedCommand CommandParser::parseClearTable(const std::string& input) {
    ParsedCommand cmd;
    cmd.type = CommandType::CLEAR_TABLE;

    size_t pos = input.find(' ') + 1;
    cmd.tableName = trim(input.substr(pos));
    return cmd;
}

// === METODY POMOCNICZE ===
std::string CommandParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string CommandParser::toUpper(const std::string& str) {
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

std::vector<std::string> CommandParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    char quoteChar = 0;

    for (char c : str) {
        if ((c == '\'' || c == '"') && !inQuotes) {
            inQuotes = true;
            quoteChar = c;
            token += c;
        } else if (inQuotes && c == quoteChar) {
            inQuotes = false;
            token += c;
        } else if (!inQuotes && c == delimiter) {
            if (!token.empty()) {
                tokens.push_back(trim(token));
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(trim(token));
    return tokens;
}

bool CommandParser::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

std::string CommandParser::extractBetweenParentheses(const std::string& str) {
    size_t start = str.find('(');
    size_t end = str.rfind(')');
    if (start == std::string::npos || end == std::string::npos || start >= end) return "";
    return str.substr(start + 1, end - start - 1);
}

std::map<std::string, std::string> CommandParser::parseWhereClause(const std::string& clause) {
    std::map<std::string, std::string> cond;
    size_t eq = clause.find('=');
    if (eq == std::string::npos) return cond;

    std::string key = trim(clause.substr(0, eq));
    std::string val = trim(clause.substr(eq + 1));
    if (!val.empty() && (val.front() == '\'' || val.front() == '"')) {
        val = val.substr(1, val.size() - 2);
    }
    cond[key] = val;
    return cond;
}

bool CommandParser::isValidCommand(const std::string& input) {
    return parse(input).type != CommandType::UNKNOWN;
}