#include "../../include/QueryExecutor.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

// === KONSTRUKTOR / DESTRUKTOR ===
QueryExecutor::QueryExecutor(const std::string& databasePath)
    : databasePath(databasePath), currentDatabase("") {

    if (!FileManager::exists(databasePath)) {
        FileManager::createDirectory(databasePath);
    }
    Logger::info("QueryExecutor: Zainicjalizowano (katalog: " + databasePath + ")");
}

QueryExecutor::~QueryExecutor() {
    saveAllTables();
    Logger::info("QueryExecutor: Zapisano wszystkie tabele i zamkniecie");
}

// === GŁÓWNA FUNKCJA WYKONUJĄCA ===
bool QueryExecutor::execute(const ParsedCommand& cmd) {
    switch (cmd.type) {
        // === DATABASE MANAGEMENT ===
        case CommandType::SHOW_DATABASES:
            return executeShowDatabases();
        case CommandType::USE_DATABASE:
            return executeUseDatabase(cmd);
        case CommandType::DROP_DATABASE:
            return executeDropDatabase(cmd);
        case CommandType::CHANGE_DATABASE_NAME:
            return executeChangeDatabaseName(cmd);

        // === DDL ===
        case CommandType::SHOW_TABLES:
            return executeShowTables();
        case CommandType::CREATE_TABLE:
            return executeCreateTable(cmd);
        case CommandType::DROP_TABLE:
            return executeDropTable(cmd);
        case CommandType::DESCRIBE_TABLE:
            return executeDescribeTable(cmd);
        case CommandType::MODIFY_TABLE_NAME:
            return executeModifyTableName(cmd);
        case CommandType::ADD_COLUMN:
            return executeAddColumn(cmd);
        case CommandType::MODIFY_COLUMN_NAME:
            return executeModifyColumnName(cmd);
        case CommandType::DROP_COLUMN:
            return executeDropColumn(cmd);

        // === DML ===
        case CommandType::INSERT:
            return executeInsert(cmd);
        case CommandType::SELECT_ALL:
            return executeSelectAll(cmd);
        case CommandType::SELECT:
            return executeSelect(cmd);
        case CommandType::UPDATE:
            return executeUpdate(cmd);
        case CommandType::DELETE:
            return executeDelete(cmd);
        case CommandType::CLEAR_TABLE:
            return executeClearTable(cmd);

        // === UTILITY ===
        case CommandType::HELP:
            showHelp();
            return true;
        case CommandType::EXIT:
            return true;

        case CommandType::UNKNOWN:
            std::cout << "Nieznana komenda. Wpisz HELP.\n";
            return false;

        default:
            std::cout << "Komenda nie zaimplementowana.\n";
            return false;
    }
}

// === DATABASE MANAGEMENT ===
bool QueryExecutor::executeShowDatabases() {
    try {
        auto dirs = FileManager::listDirectories(databasePath);
        std::cout << "\n=== Dostepne bazy danych ===\n";
        if (dirs.empty()) {
            std::cout << "Brak baz danych.\n";
        } else {
            for (const auto& db : dirs) {
                std::cout << "  " << (db == currentDatabase ? "* " : "  ") << db << "\n";
            }
            std::cout << "\nLiczba baz: " << dirs.size() << "\n";
        }
        std::cout << std::endl;
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad SHOW DATABASES: " + std::string(e.what()));
        return false;
    }
}

bool QueryExecutor::executeUseDatabase(const ParsedCommand& cmd) {
    if (cmd.databaseName.empty()) {
        std::cout << "Podaj nazwe bazy: USE nazwa_bazy\n";
        return false;
    }

    std::string dbPath = databasePath + "/" + cmd.databaseName;
    if (!FileManager::exists(dbPath)) {
        FileManager::createDirectory(dbPath);
        std::cout << "Utworzono nowa baze: " << cmd.databaseName << "\n";
    }

    saveAllTables();
    clearCache();
    currentDatabase = cmd.databaseName;
    std::cout << "Przelaczono na baze: " << currentDatabase << "\n\n";
    return true;
}

bool QueryExecutor::executeDropDatabase(const ParsedCommand& cmd) {
    if (cmd.databaseName.empty()) {
        std::cout << "Podaj nazwe bazy: DROPDATABASE nazwa\n";
        return false;
    }

    if (cmd.databaseName == currentDatabase) {
        std::cout << "Nie mozesz usunac aktualnej bazy. Najpierw przejdź na inną.\n";
        return false;
    }

    std::string dbPath = databasePath + "/" + cmd.databaseName;
    if (!FileManager::exists(dbPath)) {
        std::cout << "Baza '" << cmd.databaseName << "' nie istnieje.\n";
        return false;
    }

    try {
        FileManager::removeDirectory(dbPath);
        std::cout << "Baza '" << cmd.databaseName << "' usunieta.\n";
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad usuwania bazy: " + std::string(e.what()));
        return false;
    }
}

bool QueryExecutor::executeChangeDatabaseName(const ParsedCommand& cmd) {
    if (cmd.oldName.empty() || cmd.newName.empty()) {
        std::cout << "Uzyj: CHANGEDBNAME stara_nazwa nowa_nazwa\n";
        return false;
    }

    if (cmd.oldName == currentDatabase) {
        std::cout << "Nie mozesz zmienic nazwy aktualnej bazy.\n";
        return false;
    }

    std::string oldPath = databasePath + "/" + cmd.oldName;
    std::string newPath = databasePath + "/" + cmd.newName;

    if (!FileManager::exists(oldPath)) {
        std::cout << "Baza '" << cmd.oldName << "' nie istnieje.\n";
        return false;
    }
    if (FileManager::exists(newPath)) {
        std::cout << "Baza '" << cmd.newName << "' juz istnieje.\n";
        return false;
    }

    try {
        FileManager::rename(oldPath, newPath);
        std::cout << "Zmieniono nazwe bazy: " << cmd.oldName << " -> " << cmd.newName << "\n";
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad zmiany nazwy: " + std::string(e.what()));
        return false;
    }
}

// === DDL ===
bool QueryExecutor::executeShowTables() {
    if (currentDatabase.empty()) {
        std::cout << "Nie wybrano bazy. Uzyj: USE nazwa_bazy\n";
        return false;
    }

    try {
        std::string dbPath = getCurrentDatabasePath();
        auto files = FileManager::listFiles(dbPath);
        std::cout << "\n=== Tabele w bazie '" << currentDatabase << "' ===\n";
        int count = 0;
        for (const auto& f : files) {
            if (f.size() > 5 && f.substr(f.size() - 5) == ".sttb") {
                std::string name = f.substr(0, f.size() - 5);
                std::cout << "  - " << name << "\n";
                count++;
            }
        }
        std::cout << "\nLiczba tabel: " << count << "\n\n";
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad SHOW TABLES: " + std::string(e.what()));
        return false;
    }
}

bool QueryExecutor::executeCreateTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        std::cout << "Nie wybrano bazy.\n";
        return false;
    }
    if (cmd.tableName.empty() || cmd.columns.empty()) {
        std::cout << "Podaj nazwe i kolumny: CREATETABLE nazwa (id INT, ...)\n";
        return false;
    }
    if (tableExists(cmd.tableName)) {
        std::cout << "Tabela '" << cmd.tableName << "' juz istnieje.\n";
        return false;
    }

    try {
        std::string filePath = getTableFilePath(cmd.tableName);
        auto table = std::make_shared<Table>(cmd.tableName, filePath);
        table->init();

        for (size_t i = 0; i < cmd.columns.size(); ++i) {
            uint32_t size = 0;
            ColumnType type = parseColumnType(cmd.columnTypes[i], size);
            Column col(cmd.columns[i], type, size);
            table->createColumn(col);
        }

        table->save();
        tables[cmd.tableName] = table;
        std::cout << "Tabela '" << cmd.tableName << "' utworzona.\n\n";
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad tworzenia tabeli: " + std::string(e.what()));
        return false;
    }
}

bool QueryExecutor::executeDropTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    if (!tableExists(cmd.tableName)) {
        std::cout << "Tabela '" << cmd.tableName << "' nie istnieje.\n";
        return false;
    }

    tables.erase(cmd.tableName);
    FileManager::deleteFile(getTableFilePath(cmd.tableName));
    std::cout << "Tabela '" << cmd.tableName << "' usunieta.\n\n";
    return true;
}

bool QueryExecutor::executeDescribeTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) {
        std::cout << "Tabela '" << cmd.tableName << "' nie istnieje.\n";
        return false;
    }

    table->printStructure();
    table->printStats();
    std::cout << std::endl;
    return true;
}

bool QueryExecutor::executeModifyTableName(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    if (!tableExists(cmd.oldName)) {
        std::cout << "Tabela '" << cmd.oldName << "' nie istnieje.\n";
        return false;
    }
    if (tableExists(cmd.newName)) {
        std::cout << "Tabela '" << cmd.newName << "' juz istnieje.\n";
        return false;
    }

    std::string oldPath = getTableFilePath(cmd.oldName);
    std::string newPath = getTableFilePath(cmd.newName);
    FileManager::rename(oldPath, newPath);

    if (tables.count(cmd.oldName)) {
        tables[cmd.newName] = tables[cmd.oldName];
        tables[cmd.newName]->setName(cmd.newName);
        tables.erase(cmd.oldName);
    }

    std::cout << "Zmieniono nazwe: " << cmd.oldName << " -> " << cmd.newName << "\n\n";
    return true;
}

bool QueryExecutor::executeAddColumn(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) {
        std::cout << "Tabela '" << cmd.tableName << "' nie istnieje.\n";
        return false;
    }

    uint32_t size = 0;
    ColumnType type = parseColumnType(cmd.columnType, size);
    Column col(cmd.columnName, type, size);
    table->createColumn(col);
    table->save();

    std::cout << "Dodano kolumne '" << cmd.columnName << "' do '" << cmd.tableName << "'.\n\n";
    return true;
}

bool QueryExecutor::executeModifyColumnName(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    int idx = table->findColumnByName(cmd.oldName);
    if (idx == -1) {
        std::cout << "Kolumna '" << cmd.oldName << "' nie istnieje.\n";
        return false;
    }

    table->renameColumn(idx, cmd.newName);
    table->save();
    std::cout << "Zmieniono kolumne: " << cmd.oldName << " -> " << cmd.newName << "\n\n";
    return true;
}

bool QueryExecutor::executeDropColumn(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    int idx = table->findColumnByName(cmd.columnName);
    if (idx == -1) {
        std::cout << "Kolumna '" << cmd.columnName << "' nie istnieje.\n";
        return false;
    }

    table->dropColumn(idx);
    table->save();
    std::cout << "Usunieto kolumne '" << cmd.columnName << "'.\n\n";
    return true;
}

// === DML ===
bool QueryExecutor::executeInsert(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    int32_t id = table->insertRowFromStrings(cmd.values);
    if (id != -1) {
        std::cout << "Wstawiono wiersz o ID: " << id << "\n\n";
        return true;
    }
    return false;
}

bool QueryExecutor::executeSelectAll(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    table->printData();
    std::cout << std::endl;
    return true;
}

bool QueryExecutor::executeSelect(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    if (!cmd.conditions.empty()) {
        auto ids = findMatchingRows(table, cmd.conditions);
        if (ids.empty()) {
            std::cout << "Brak wierszy spelniajacych warunki.\n\n";
            return true;
        }

        std::cout << "\n=== Wyniki SELECT ===\n";
        std::cout << std::setw(8) << "Row ID";
        for (const auto& col : table->getColumns()) {
            std::cout << " | " << std::setw(15) << col.getName();
        }
        std::cout << "\n" << std::string(80, '-') << "\n";

        for (int32_t id : ids) {
            const Row* row = table->selectByID(id);
            if (row) row->print(table->getColumns());
        }
        std::cout << "\nZnaleziono: " << ids.size() << " wierszy.\n\n";
    } else {
        table->printData();
    }
    return true;
}

bool QueryExecutor::executeUpdate(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    if (cmd.conditions.empty()) {
        std::cout << "UPDATE wymaga WHERE.\n";
        return false;
    }

    auto table = getTable(cmd.tableName);
    if (!table) return false;

    int colIdx = table->findColumnByName(cmd.columns[0]);
    if (colIdx == -1) {
        std::cout << "Kolumna '" << cmd.columns[0] << "' nie istnieje.\n";
        return false;
    }

    auto ids = findMatchingRows(table, cmd.conditions);
    int count = 0;
    for (int32_t id : ids) {
        CellValue val = Row::parseValue(cmd.values[0], table->getColumn(colIdx)->getType());
        if (table->updateCell(id, cmd.columns[0], val)) count++;
    }

    std::cout << "Zaktualizowano: " << count << " wierszy.\n\n";
    return true;
}

bool QueryExecutor::executeDelete(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    if (cmd.conditions.empty()) {
        std::cout << "DELETE wymaga WHERE.\n";
        return false;
    }

    auto table = getTable(cmd.tableName);
    if (!table) return false;

    auto ids = findMatchingRows(table, cmd.conditions);
    int count = 0;
    for (int32_t id : ids) {
        if (table->deleteRow(id)) count++;
    }

    std::cout << "Usunieto: " << count << " wierszy.\n\n";
    return true;
}

bool QueryExecutor::executeClearTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    table->clearData();
    table->save();
    std::cout << "Tabela '" << cmd.tableName << "' wyczyszczona.\n\n";
    return true;
}

// === UTILITY ===
void QueryExecutor::showHelp() {
    std::cout << R"(
=== STUPIDDB - POMOC ===
  
  SHOWDATABASES                - lista baz
  USE nazwa                    - przelacz baze
  DROPDATABASE nazwa           - usun baze
  CHANGEDBNAME stara nowa      - zmien nazwe bazy

  SHOWTABLES                   - lista tabel
  CREATETABLE nazwa (id INT, ...) - utworz tabele
  DROPTABLE nazwa              - usun tabele
  DESCRIBE nazwa               - struktura tabeli
  MODIFYTABLE stara nowa       - zmien nazwe tabeli
  ADDCOLUMN tabela kol typ     - dodaj kolumne
  MODIFYCOLUMN tabela stara nowa - zmien nazwe kolumny
  DROPCOLUMN tabela kol        - usun kolumne

  INSERTINTO tabela VALUES (...) - wstaw dane
  SELECTALL tabela             - wszystkie dane
  SELECT tabela WHERE kol=wart - filtruj
  UPDATE tabela SET kol=wart WHERE ... - aktualizuj
  DELETE tabela WHERE kol=wart - usun
  CLEARTABLE tabela            - wyczysc tabele

  HELP                         - ta pomoc
  EXIT                         - wyjscie
)";
    std::cout << std::endl;
}

// === METODY POMOCNICZE ===
std::shared_ptr<Table> QueryExecutor::getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it != tables.end()) return it->second;

    std::string path = getTableFilePath(tableName);
    if (!FileManager::exists(path)) return nullptr;

    try {
        auto table = std::make_shared<Table>(tableName, path);
        table->load();
        tables[tableName] = table;
        return table;
    } catch (...) {
        return nullptr;
    }
}

ColumnType QueryExecutor::parseColumnType(const std::string& typeStr, uint32_t& size) {
    std::string upper = typeStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper == "INT") { size = 0; return ColumnType::INT; }
    if (upper == "FLOAT") { size = 0; return ColumnType::FLOAT; }
    if (upper == "BOOL" || upper == "BOOLEAN") { size = 0; return ColumnType::BOOL; }

    if (upper.find("VARCHAR") == 0) {
        size_t s = upper.find('(');
        size_t e = upper.find(')');
        if (s != std::string::npos && e != std::string::npos) {
            size = std::stoi(upper.substr(s + 1, e - s - 1));
        } else {
            size = 255;
        }
        return ColumnType::VARCHAR;
    }

    size = 255;
    return ColumnType::VARCHAR;
}

std::string QueryExecutor::getTableFilePath(const std::string& tableName) const {
    return getCurrentDatabasePath() + "/" + tableName + ".sttb";
}

std::string QueryExecutor::getCurrentDatabasePath() const {
    return currentDatabase.empty() ? databasePath : databasePath + "/" + currentDatabase;
}

bool QueryExecutor::tableExists(const std::string& tableName) const {
    return FileManager::exists(getTableFilePath(tableName));
}

void QueryExecutor::saveAllTables() {
    for (auto& p : tables) {
        try { p.second->save(); } catch (...) {}
    }
}

void QueryExecutor::clearCache() {
    tables.clear();
}

std::vector<int32_t> QueryExecutor::findMatchingRows(std::shared_ptr<Table> table,
                                                     const std::map<std::string, std::string>& conditions) {
    std::vector<int32_t> result;
    if (conditions.empty()) return result;

    auto rows = table->selectAll();
    for (const auto& row : rows) {
        bool match = true;
        for (const auto& cond : conditions) {
            int idx = table->findColumnByName(cond.first);
            if (idx == -1 || Row::cellToString(row.getCell(idx)) != cond.second) {
                match = false;
                break;
            }
        }
        if (match) result.push_back(row.getRowID());
    }
    return result;
}