#include "../../include/QueryExecutor.hpp"
#include "../../include/Logger.hpp"
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <filesystem>

using json = nlohmann::json;

std::shared_mutex QueryExecutor::m;

// === ODPOWIEDŹ NA JSON ===
json QueryResponse::rowsToJSON() {
    json j;
    for(size_t i = 0; i < rows.size(); i++) {
        for(size_t k = 0; k < rows[i].size(); k++) {
            std::visit([&](auto&& val) {
                j[i][k] = val;
            } , rows[i][k]);
        }
    }
    return j;
}

std::string QueryResponse::toJSON() {
    json j;
    switch(type) {
        case QueryType::MESSAGE_:
            j["resType"] = "MESSAGE";
            j["message"] = message;
            break;
        case QueryType::ERROR_:
            j["resType"] = "ERROR";
            j["message"] = message;
            break;
        case QueryType::TABLE_:
            j["resType"] = "TABLE";
            j["cols"] = cols;
            j["rows"] = rowsToJSON();
            break;
    }

    return j.dump();
}

// === HELPERY DLA ODPOWIEDZI ===
QueryResponse QueryResponse::genError(HTTP_Status status, std::string message) {
    QueryResponse res;
    res.status = status;
    res.type = QueryType::ERROR_;
    res.message = message;

    return res;
}

QueryResponse QueryResponse::genMessage(std::string message) {
    QueryResponse res;
    res.status = HTTP_Status::OK;
    res.type = QueryType::MESSAGE_;
    res.message = message;

    return res;
}

QueryResponse QueryResponse::genTable(std::vector<std::string> cols, std::vector<RowValues> rows) {
    QueryResponse res;
    res.status = HTTP_Status::OK;
    res.type = QueryType::TABLE_;
    res.cols = cols;
    res.rows = rows;

    return res;
}


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
QueryResponse QueryExecutor::execute(const ParsedCommand& cmd) {
    switch (cmd.type) {
        // === DATABASE MANAGEMENT ===
        case CommandType::SHOW_DATABASES_: {
            std::shared_lock lock(m);
            return executeShowDatabases();
        }
        case CommandType::USE_DATABASE_: {
            std::unique_lock lock(m);
            return executeUseDatabase(cmd);
        }

        case CommandType::DROP_DATABASE_: {
            std::unique_lock lock(m);
            return executeDropDatabase(cmd);
        }

        // === DDL ===
        case CommandType::SHOW_TABLES_: {
            std::shared_lock lock(m);
            return executeShowTables();
        }
            
        case CommandType::CREATE_TABLE_: {
            std::unique_lock lock(m);
            return executeCreateTable(cmd);
        }
            
        case CommandType::DROP_TABLE_: {
            std::unique_lock lock(m);
            return executeDropTable(cmd);
        }

        case CommandType::DESCRIBE_TABLE_: {
            std::shared_lock lock(m);
            return executeDescribeTable(cmd);
        }
            
        case CommandType::MODIFY_TABLE_NAME_: {
            std::unique_lock lock(m);
            return executeModifyTableName(cmd);
        }
            
        case CommandType::ADD_COLUMN_: {
            std::unique_lock lock(m);
            return executeAddColumn(cmd);
        }

        case CommandType::MODIFY_COLUMN_NAME_: {
            std::unique_lock lock(m);
            return executeModifyColumnName(cmd);
        }
            
        case CommandType::DROP_COLUMN_: {
            std::unique_lock lock(m);
            return executeDropColumn(cmd);
        }

        // === DML ===
        case CommandType::INSERT_: {
            std::unique_lock lock(m);
            return executeInsert(cmd);
        }
            
        case CommandType::SELECT_ALL_: {
            std::shared_lock lock(m);
            return executeSelectAll(cmd);
        }
           
        case CommandType::SELECT_: {
            std::shared_lock lock(m);
            return executeSelect(cmd);
        }
            
        case CommandType::UPDATE_: {
            std::unique_lock lock(m);
            return executeUpdate(cmd);
        }
            
        case CommandType::DELETE_: {
            std::unique_lock lock(m);
            return executeDelete(cmd);
        }
            
        case CommandType::CLEAR_TABLE_: {
            std::unique_lock lock(m);
            return executeClearTable(cmd);
        }

        // === UTILITY ===
        case CommandType::HELP_:
            return QueryResponse::genMessage(getHelp());
        
        case CommandType::USENONE_: {
            std::unique_lock lock(m);
            return executeUseNone();
        }

        case CommandType::UNKNOWN_:
            return QueryResponse::genError(HTTP_Status::NotFound, "Nieznana komenda. Wpisz HELP");

        default:
            return QueryResponse::genError(HTTP_Status::NotFound, "Komenda nie zaimplementowana");
    }
}

// === DATABASE MANAGEMENT ===
QueryResponse QueryExecutor::executeUseNone() {
    if (currentDatabase.empty()) {
        return QueryResponse::genMessage("Juz nie jestes w zadnej bazie.");
    } else {
        saveAllTables();
        clearCache();
        currentDatabase = "";
        return QueryResponse::genMessage("Wylogowano z bazy.");
    }
}

std::string QueryExecutor::findExactDatabaseName(const std::string& dbName) const {
    try {
        auto dirs = FileManager::listDirectories(databasePath);
        
        for (const auto& dir : dirs) {
            if (dir == dbName) {
                return dir; 
            }
        }
        
        // std::string lowerDbName = dbName;
        // std::transform(lowerDbName.begin(), lowerDbName.end(), lowerDbName.begin(), ::tolower);
        
        for (const auto& dir : dirs) {
            // std::string lowerDir = dir;
            // std::transform(lowerDir.begin(), lowerDir.end(), lowerDir.begin(), ::tolower);
            
            if (dir == dbName) {
                return dir;  
            }
        }
        
        return "";  
    } catch (const std::exception& e) {
        Logger::error("Blad wyszukiwania bazy: " + std::string(e.what()));
        return "";
    }
}

QueryResponse QueryExecutor::executeShowDatabases() {
    try {
        auto dirs = FileManager::listDirectories(databasePath);
        if (dirs.empty()) {
            return QueryResponse::genMessage("Brak baz danych.");
        }

        std::vector<RowValues> rows;
        for (const auto& db : dirs) {
            rows.push_back({ db });
        }

        return QueryResponse::genTable({ "Bazy danych" }, rows);
    } catch (const std::exception& e) {
       return QueryResponse::genError(HTTP_Status::InternalServerError, "Blad SHOW DATABASES: " + std::string(e.what()));
    }
}

QueryResponse QueryExecutor::executeUseDatabase(const ParsedCommand& cmd) {
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
        // if (currentDatabase == exactName) {
        //     if (exactName != dbName) {
        //         Logger::warn("Juz jestes w bazie '" + exactName + "'RowValuesRowValues");
        //     }
        // }
        
        saveAllTables();
        clearCache();
        currentDatabase = exactName;

        return QueryResponse::genMessage("Przelaczono na baze: " + exactName);
    } else {
        std::string dbPath = databasePath + "/" + dbName;
        FileManager::createDirectory(dbPath);
        
        saveAllTables();
        clearCache();
        currentDatabase = dbName;
        return QueryResponse::genMessage("Utworzono baze: " + dbName);
    }
}

QueryResponse QueryExecutor::executeDropDatabase(const ParsedCommand& cmd) {
    if (cmd.databaseName.empty()) {
        return QueryResponse::genError(HTTP_Status::BadRequest, "Podaj nazwe bazy: DROPDATABASE nazwa");
    }

    std::string dbName = cmd.databaseName;
    if (!dbName.empty() && (dbName.front() == '"' || dbName.front() == '\'')) {
        dbName = dbName.substr(1, dbName.size() - 2);
    }

    std::string exactName = findExactDatabaseName(dbName);
        
    if (exactName.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza '" + dbName + "' nie istnieje.");
    }

    if (exactName == currentDatabase) {
        return QueryResponse::genError(HTTP_Status::BadRequest, "Nie mozesz zmienic nazwy aktualnej bazy na podobna inna lub bedac w niej.");
    }

    std::string dbPath = databasePath + "/" + exactName;

    try {
        FileManager::removeDirectory(dbPath);
        return QueryResponse::genMessage("Baza '" +  exactName + "'zostala usunieta.");
    } catch (const std::exception& e) {
        return QueryResponse::genError(HTTP_Status::InternalServerError, "Blad usuwania bazy: " + std::string(e.what()));
    }
}

// === DDL ===
QueryResponse QueryExecutor::executeShowTables() {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound ,"Nie wybrano bazy. Uzyj: USE nazwa_bazy");
    }

    try {
        std::string dbPath = getCurrentDatabasePath();
        auto files = FileManager::listFiles(dbPath);

        std::vector<RowValues> rows;
        for (const auto& f : files) {
            if (f.size() > 5 && f.substr(f.size() - 5) == ".sttb") {
                std::string name = f.substr(0, f.size() - 5);
                rows.push_back({ name });
            }
        }

        return QueryResponse::genTable({ "Tabele" }, rows);
    } catch (const std::exception& e) {
        return  QueryResponse::genError(HTTP_Status::InternalServerError, "Blad SHOW TABLES: " + std::string(e.what()) );
    }
}

QueryResponse QueryExecutor::executeCreateTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::BadRequest, "Nie wybrano bazy. Uzyj: USE nazwa_bazy");
    }
    // if (cmd.tableName.empty() || cmd.columns.empty()) {
    //     std::cout << "Podaj nazwe i kolumny: CREATETABLE nazwa (id INT, ...)RowValues";
    //     return false;
    // }
    if (tableExists(cmd.tableName)) {
        return QueryResponse::genError(HTTP_Status::BadRequest ,"Tabela '" + cmd.tableName + "' juz istnieje.");
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

        return QueryResponse::genMessage("Baza danych utworzona pomyślnie.");
    } catch (const std::exception& e) {
        return QueryResponse::genError(HTTP_Status::InternalServerError , "Blad tworzenia tabeli: " + std::string(e.what()));
    }
}

QueryResponse QueryExecutor::executeDropTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    if (!tableExists(cmd.tableName)) {
        return QueryResponse::genError(HTTP_Status::NotFound ,"Tabela '" + cmd.tableName + "' nie istnieje.");
    }

    tables.erase(cmd.tableName);
    FileManager::deleteFile(getTableFilePath(cmd.tableName));

    return QueryResponse::genMessage("Tabela '" + cmd.tableName + "' usunieta.");
}


QueryResponse QueryExecutor::executeDescribeTable(const ParsedCommand& cmd) {
     if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
    if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound ,"Tabela '" + cmd.tableName + "' nie istnieje.");
    }

    return QueryResponse::genTable({ "Kolumny" }, table->getStructure());
}


QueryResponse QueryExecutor::executeModifyTableName(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    if (!tableExists(cmd.oldName)) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Tabela '" + cmd.oldName + "' nie istnieje.");
    }

    if (tableExists(cmd.newName)) {
        return QueryResponse::genError(HTTP_Status::BadRequest,  "Tabela '" + cmd.newName + "' juz istnieje.");
    }

    std::string oldPath = getTableFilePath(cmd.oldName);
    std::string newPath = getTableFilePath(cmd.newName);
    FileManager::rename(oldPath, newPath);

    if (tables.count(cmd.oldName)) {
        tables[cmd.newName] = tables[cmd.oldName];
        tables[cmd.newName]->setName(cmd.newName);
        tables.erase(cmd.oldName);
    }


    return QueryResponse::genMessage("Zmieniono nazwe tabeli: " + cmd.oldName + " -> " + cmd.newName);
}

QueryResponse QueryExecutor::executeAddColumn(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
    if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound ,"Tabela '" + cmd.tableName + "' nie istnieje.");;
    }

    if (table->findColumnByName(cmd.columnName) != -1) {
        return QueryResponse::genError(HTTP_Status::BadRequest,  "Tabela '" + cmd.newName + "' juz istnieje.");
    }

    uint32_t size = 0;
    ColumnType type = parseColumnType(cmd.columnType, size);
    Column col(cmd.columnName, type, size);
    
    int result = table->createColumn(col);
    if (result == -1) {
        return QueryResponse::genError(HTTP_Status::InternalServerError, "Blad tworzenia kolumny.");
    }

    table->save();

    return QueryResponse::genMessage("Dodano kolumne '" + cmd.columnName + "' do '" + cmd.tableName + "'.");
}

QueryResponse QueryExecutor::executeModifyColumnName(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
    if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }

    int idx = table->findColumnByName(cmd.oldName);
    if (idx == -1) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Kolumna '" + cmd.oldName + "' nie istnieje.");
    }

    table->renameColumn(idx, cmd.newName);
    table->save();

    return QueryResponse::genMessage("Zmieniono nazwe kolumny: " + cmd.oldName + " -> " + cmd.newName);
}

QueryResponse QueryExecutor::executeDropColumn(const ParsedCommand& cmd) {
     if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
      if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }

    int idx = table->findColumnByName(cmd.columnName);
    if (idx == -1) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Kolumna '" + cmd.oldName + "' nie istnieje.");
    }

    table->dropColumn(idx);
    table->save();

    return QueryResponse::genMessage("Usunieto kolumne '" + cmd.columnName);
}

// === DML ===
QueryResponse QueryExecutor::executeInsert(const ParsedCommand& cmd) {
     if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
      if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }


    int32_t id = table->insertRowFromStrings(cmd.values);
    if (id == -1) {
        return QueryResponse::genError(HTTP_Status::InternalServerError, "Wystapil blad podczas dodawania danych do tabeli");
    }
    return QueryResponse::genMessage(" Wstawiono wiersz o ID: " + std::to_string(id));
}

QueryResponse QueryExecutor::executeSelectAll(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
      if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }

    std::vector<std::string> colNames;
    for(auto &col: table->getColumns()) {
        colNames.push_back(col.getName());
    }
    
    std::vector<RowValues> rows;
    for(auto &row: table->selectAll()) {
        rows.push_back(row.getCells());
    }

    return QueryResponse::genTable(colNames, rows);
}

QueryResponse QueryExecutor::executeSelect(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
      if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }

    if (!cmd.conditions.empty()) {
        auto ids = findMatchingRows(table, cmd.conditions);
        if (ids.empty()) {
            return QueryResponse::genTable({ },{ });
        }

        std::vector<std::string> colNames;
        for(auto &col: table->getColumns()) {
            colNames.push_back(col.getName());
        }

        std::vector<RowValues> rows;
        for(int32_t id: ids) {
            const Row* row = table->selectByID(id);
            if(row)
                rows.push_back(row->getCells());
        }

        return QueryResponse::genTable(colNames, rows);
    } else {
        auto cmdCopy = cmd;
        cmdCopy.type = CommandType::SELECT_ALL_;
        return executeSelectAll(cmdCopy);
    }
    
}

QueryResponse QueryExecutor::executeUpdate(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    if (cmd.conditions.empty()) {
        QueryResponse::genError(HTTP_Status::BadRequest, "UPDATE wymaga WHERE.");
    }

    auto table = getTable(cmd.tableName);
    if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }
     
    int colIdx = table->findColumnByName(cmd.columns[0]);
    if (colIdx == -1) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Kolumna '" + cmd.columns[0] + "' nie istnieje.");
    }

    auto ids = findMatchingRows(table, cmd.conditions);
    int count = 0;
    for (int32_t id : ids) {
        CellValue val = Row::parseValue(cmd.values[0], table->getColumn(colIdx)->getType());
        if (table->updateCell(id, cmd.columns[0], val)) count++;
    }

    return QueryResponse::genMessage("Zaktualizowano: " + std::to_string(count) + " wierszy.");
}

QueryResponse QueryExecutor::executeDelete(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    if (cmd.conditions.empty()) {
        QueryResponse::genError(HTTP_Status::BadRequest, "DELETE wymaga WHERE.");
    }

    auto table = getTable(cmd.tableName);
    if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }

    auto ids = findMatchingRows(table, cmd.conditions);
    int count = 0;
    for (int32_t id : ids) {
        if (table->deleteRow(id)) count++;
    }

    return QueryResponse::genMessage("Usunieto: " + std::to_string(count) + " wierszy.RowValues");
}

QueryResponse QueryExecutor::executeClearTable(const ParsedCommand& cmd) {
    if (currentDatabase.empty()) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Baza danych nie jest wybrana.");
    }

    auto table = getTable(cmd.tableName);
    if (!table) {
        return QueryResponse::genError(HTTP_Status::NotFound, "Podana tabela nie istnieje");
    }

    table->clearData();
    table->save();

    return QueryResponse::genMessage("Tabela '" + cmd.tableName + "' wyczyszczona.");
}

// === UTILITY ===
std::string QueryExecutor::getHelp() {
    return  R"(
=== STUPIDDB - POMOC ===
  
  SHOWDATABASES                - lista baz
  USE nazwa                    - przelacz baze
  DROPDATABASE nazwa           - usun baze

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
  USENONE                      - nie uzywaj zadnej bazy
  EXIT                         - wyjscie
)";
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