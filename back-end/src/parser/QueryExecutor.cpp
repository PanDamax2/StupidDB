#include "../../include/QueryExecutor.hpp"
#include "../../include/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <filesystem> 

// === KONSTRUKTOR / DESTRUKTOR ===
QueryExecutor::QueryExecutor(const std::string& databasePath)
    : databasePath(databasePath), currentDatabase("") {

    if (!FileManager::exists(databasePath)) {
        FileManager::createDirectory(databasePath);
    }
    #ifdef TEST
    Logger::info("QueryExecutor: Zainicjalizowano (katalog: " + databasePath + ")");
    #endif

    std::string absolutePath = std::filesystem::absolute(databasePath).string();
    Logger::info("Folder bazy danych: " + absolutePath);
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
        
        case CommandType::LOGOUT:
            return executeLogout();

        case CommandType::UNKNOWN:
            Logger::warn("Nieznana komenda. Wpisz HELP");
            return false;

        default:
            Logger::warn("Komenda nie zaimplementowana.\n");
            return false;
    }
}

// === DATABASE MANAGEMENT ===
bool QueryExecutor::executeLogout() {
    if (currentDatabase.empty()) {
        Logger::info("Juz nie jestes w zadnej bazie.\n\n");
    } else {
        saveAllTables();
        clearCache();
        currentDatabase = "";
        Logger::info("Wylogowano z bazy.\n\n");
    }
    return true;
}

std::string QueryExecutor::findExactDatabaseName(const std::string& dbName) const {
    try {
        auto dirs = FileManager::listDirectories(databasePath);
        
        for (const auto& dir : dirs) {
            if (dir == dbName) {
                return dir; 
            }
        }
        
        std::string lowerDbName = dbName;
        std::transform(lowerDbName.begin(), lowerDbName.end(), lowerDbName.begin(), ::tolower);
        
        for (const auto& dir : dirs) {
            std::string lowerDir = dir;
            std::transform(lowerDir.begin(), lowerDir.end(), lowerDir.begin(), ::tolower);
            
            if (lowerDir == lowerDbName) {
                return dir;  
            }
        }
        
        return "";  
    } catch (const std::exception& e) {
        Logger::error("Blad wyszukiwania bazy: " + std::string(e.what()));
        return "";
    }
}

bool QueryExecutor::executeShowDatabases() {
    try {
        auto dirs = FileManager::listDirectories(databasePath);
        std::cout << std::endl;
        Logger::info("=== Dostepne bazy danych ===");
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
    std::string dbName = cmd.databaseName;
    if (!dbName.empty() && (dbName.front() == '"' || dbName.front() == '\'')) {
        if (dbName.back() == dbName.front()) {
            dbName = dbName.substr(1, dbName.size() - 2);
        }
    }
    
    std::string exactName = findExactDatabaseName(dbName);
    
    if (!exactName.empty()) {
        // if (exactName != dbName) {
        //     Logger::warn("Uwaga: Baza istnieje jako '" + exactName + "' (inna wielkosc liter)");
        // }
        if (currentDatabase == exactName) {
            if (exactName != dbName) {
                Logger::warn("Juz jestes w bazie '" + exactName + "' (inna wielkosc liter)\n\n");
            } else {
                Logger::warn("Juz jestes w bazie '" + exactName + "'\n\n");
            }
            return true;
        }

         if (exactName != dbName) {
            Logger::warn("Uwaga: Baza istnieje jako '" + exactName + "' (inna wielkosc liter)");
        }
        
        saveAllTables();
        clearCache();
        currentDatabase = exactName;
        Logger::info("Przelaczono na baze: " + exactName);
    } else {
        std::string dbPath = databasePath + "/" + dbName;
        FileManager::createDirectory(dbPath);
        
        saveAllTables();
        clearCache();
        currentDatabase = dbName;
        Logger::info("Utworzono baze: " + dbName);
    }
    
    std::cout << std::endl;
    return true;
}

bool QueryExecutor::executeDropDatabase(const ParsedCommand& cmd) {
    if (cmd.databaseName.empty()) {
        std::cout << "Podaj nazwe bazy: DROPDATABASE nazwa\n";
        return false;
    }

    std::string dbName = cmd.databaseName;
    if (!dbName.empty() && (dbName.front() == '"' || dbName.front() == '\'')) {
        dbName = dbName.substr(1, dbName.size() - 2);
    }

    std::string exactName = findExactDatabaseName(dbName);
    
    if (exactName.empty()) {
        std::cout << "Baza '" << dbName << "' nie istnieje.\n";
        return false;
    }

    if (exactName == currentDatabase) {
        Logger::error("Nie mozesz zmienic nazwy aktualnej bazy na podobna inna lub bedac w niej.");
        return false;
    }

    std::string dbPath = databasePath + "/" + exactName;

    try {
        FileManager::removeDirectory(dbPath);
        std::cout << "Baza '" << exactName << "'zostala usunieta.\n";
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

    std::string exactOldName = findExactDatabaseName(cmd.oldName);
    
    if (exactOldName.empty()) {
        std::cout << "Baza '" << cmd.oldName << "' nie istnieje.\n";
        return false;
    }

    if (exactOldName == currentDatabase) {
        Logger::error("Nie mozesz zmienic nazwy aktualnej bazy na podobna inna lub bedac w niej.");
        return false;
    }

    std::string exactNewName = findExactDatabaseName(cmd.newName);
    if (!exactNewName.empty()) {
        std::cout << "Baza '" << exactNewName << "' juz istnieje.\n";
        return false;
    }

    std::string oldPath = databasePath + "/" + exactOldName;
    std::string newPath = databasePath + "/" + cmd.newName;

    try {
        FileManager::rename(oldPath, newPath);
        Logger::info("Zmieniono nazwe bazy: " + exactOldName + " -> " + cmd.newName);
        std::cout << std::endl;
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad zmiany nazwy: " + std::string(e.what()));
        return false;
    }
}

// === DDL ===
bool QueryExecutor::executeShowTables() {
    if (currentDatabase.empty()) {
        Logger::warn("Nie wybrano bazy. Uzyj: USE nazwa_bazy");
        return false;
    }

    try {
        std::string dbPath = getCurrentDatabasePath();
        auto files = FileManager::listFiles(dbPath);

        std::cout << std::endl;
        Logger::info("=== Tabele w bazie '" + currentDatabase + "' ===");
        int count = 0;
        for (const auto& f : files) {
            if (f.size() > 5 && f.substr(f.size() - 5) == ".sttb") {
                std::string name = f.substr(0, f.size() - 5);
                std::cout << "  - " << name << "\n";
                count++;
            }
        }
        std::cout << "Liczba tabel: " << count << "\n\n";
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad SHOW TABLES: " + std::string(e.what()));
        return false;
    }
}

bool QueryExecutor::executeCreateTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        Logger::warn("Nie wybrano bazy. Uzyj: USE nazwa_bazy");
        return false;
    }
    // if (cmd.tableName.empty() || cmd.columns.empty()) {
    //     std::cout << "Podaj nazwe i kolumny: CREATETABLE nazwa (id INT, ...)\n";
    //     return false;
    // }
    if (tableExists(cmd.tableName)) {
        Logger::warn("Tabela '" + cmd.tableName + "' juz istnieje.");
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
        Logger::info("Tabela '" + cmd.tableName + "' utworzona. \n");
        return true;
    } catch (const std::exception& e) {
        Logger::error("Blad tworzenia tabeli: " + std::string(e.what()));
        return false;
    }
}

bool QueryExecutor::executeDropTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    if (!tableExists(cmd.tableName)) {
        Logger::warn("Tabela '" + cmd.tableName + "' nie istnieje.");
        return false;
    }

    tables.erase(cmd.tableName);
    FileManager::deleteFile(getTableFilePath(cmd.tableName));
    Logger::info("Tabela '" + cmd.tableName + "' usunieta.");
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

    Logger::info("Zmieniono nazwe tabeli: " + cmd.oldName + " -> " + cmd.newName);
    std::cout << std::endl;
    return true;
}

bool QueryExecutor::executeAddColumn(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) {
        Logger::warn("Tabela '" + cmd.tableName + "' nie istnieje.");
        return false;
    }

    if (table->findColumnByName(cmd.columnName) != -1) {
        Logger::warn("Kolumna '" + cmd.columnName + "' juz istnieje.");
        return false;
    }

    uint32_t size = 0;
    ColumnType type = parseColumnType(cmd.columnType, size);
    Column col(cmd.columnName, type, size);
    
    int result = table->createColumn(col);
    if (result == -1) {
        // Logger::warn("Blad dodawania tworzenia kolumny.");
        return false;
    }

    table->save();

    Logger::info("Dodano kolumne '" + cmd.columnName + "' do '" + cmd.tableName + "'.\n");
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

    Logger::info("Zmieniono nazwe kolumny: " + cmd.oldName + " -> " + cmd.newName);
    std::cout << std::endl;
    return true;
}

bool QueryExecutor::executeDropColumn(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    int idx = table->findColumnByName(cmd.columnName);
    if (idx == -1) {
        Logger::warn("Kolumna '" + cmd.columnName + "' nie istnieje.");
        return false;
    }

    table->dropColumn(idx);
    table->save();

    Logger::info("Usunieto kolumne '" + cmd.columnName + "'.\n");
    return true;
}

// === DML ===
bool QueryExecutor::executeInsert(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    int32_t id = table->insertRowFromStrings(cmd.values);
    if (id != -1) {
        Logger::info("Wstawiono wiersz o ID: " + std::to_string(id));
        std::cout << std::endl;
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
            Logger::warn("Brak wierszy spelniajacych warunki.\n");
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
        Logger::warn("UPDATE wymaga WHERE.");
        return false;
    }

    auto table = getTable(cmd.tableName);
    if (!table) return false;
     
    int colIdx = table->findColumnByName(cmd.columns[0]);
    if (colIdx == -1) {
        Logger::warn("Kolumna '" + cmd.columns[0] + "' nie istnieje.");
        return false;
    }

    auto ids = findMatchingRows(table, cmd.conditions);
    int count = 0;
    for (int32_t id : ids) {
        CellValue val = Row::parseValue(cmd.values[0], table->getColumn(colIdx)->getType());
        if (table->updateCell(id, cmd.columns[0], val)) count++;
    }

    Logger::info("Zaktualizowano: " + std::to_string(count) + " wierszy.\n");
    return true;
}

bool QueryExecutor::executeDelete(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    if (cmd.conditions.empty()) {
        Logger::warn("DELETE wymaga WHERE.");
        return false;
    }

    auto table = getTable(cmd.tableName);
    if (!table) return false;

    auto ids = findMatchingRows(table, cmd.conditions);
    int count = 0;
    for (int32_t id : ids) {
        if (table->deleteRow(id)) count++;
    }

    Logger::info("Usunieto: " + std::to_string(count) + " wierszy.\n");
    return true;
}

bool QueryExecutor::executeClearTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) return false;
    auto table = getTable(cmd.tableName);
    if (!table) return false;

    table->clearData();
    table->save();

    Logger::info("Tabela '" + cmd.tableName + "' wyczyszczona.\n");
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
  LOGOUT                       - wyloguj
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