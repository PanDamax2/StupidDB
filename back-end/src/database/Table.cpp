#include "../../include/database/Table.hpp"
#include "../../include/FileManager.hpp"
#include "../../include/Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

// === KONSTRUKTOR HEADERA ===
// Ustawiamy sygnaturę STTB
TableHeader::TableHeader() {
    signature[0] = 'S'; signature[1] = 'T';
    signature[2] = 'T'; signature[3] = 'B';
    
    colsCount = rowsCount = currentRowID = 0;
}

// === KONSTRUKTOR / DESTRUKTOR ===
// Inicjalizuje nazwę tabeli, nazwę pliku i nagłówek
Table::Table(const std::string& tableName, const std::string& tableFile): tableName(tableName), tableFile(tableFile), isModified(false) {
    
    header = TableHeader();
    
    #ifdef TEST
    Logger::info("Utworzono objekt tabeli: " + tableName);
    #endif
}

// Jeśli tabela była modyfikowana, zapisujemy zmiany
Table::~Table() {
    if (isModified) {
        #ifdef TEST
        Logger::info("Zapisywanie zmian w tabeli " + tableName);
        #endif

        try {
            save();
        } catch (const std::exception& e) {
            Logger::error("Blad zapisu w destruktorze: " + std::string(e.what()));
        }
    }
}

// === INICJALIZACJA ===
bool Table::init() {
    // Sprawdzamy czy plik tabeli istnieje
    if (FileManager::exists(tableFile)) {
        #ifdef TEST
        Logger::info("Odczytywanie istniejącej tabeli: " + tableName);
        #endif
        return load();
    } else {
        // Zapisujemy pustą tabelę
        #ifdef TEST
        Logger::info("Tworzenie nowej tabeli: " + tableName);
        #endif
        return save(); 
    }
}

// === OPERACJE NA KOLUMNACH ===
int Table::createColumn(const Column& column) {
    // Sprawdzamy limit kolumn
    if (header.colsCount >= MAX_COLS_COUNT) {
        Logger::error("Table: Osiagnieto maksymalna liczbe kolumn (" + std::to_string(MAX_COLS_COUNT) + ")");
        return -1;
    }
    
    // Sprawdzamy czy kolumna o tej nazwie już istnieje
    if (findColumnByName(column.getName()) != -1) {
        Logger::error("Table: Kolumna '" + column.getName() + "' juz istnieje");
        return -1;
    }
    
    // Jeśli tabela ma już dane, nie można dodawać kolumn
    if (header.rowsCount > 0) {
        Logger::error("Table: Nie mozna dodac kolumny do tabeli z danymi");
        return -1;
    }
    
    // Dodajemy kolumnę
    columns.push_back(column);
    header.colsCount++;
    isModified = true;
    
    #ifdef TEST
    Logger::info("Dodano kolumne: " + column.getName() + " (" + column.typeToString() + ")");
    #endif
    return static_cast<int>(columns.size() - 1);
}

// Usuwa kolumnę jeśli tabela jest pusta
bool Table::deleteColumn(const std::string& columnName) {
    int index = findColumnByName(columnName);
    
    if (index == -1) {
        Logger::error("Table: Kolumna '" + columnName + "' nie istnieje");
        return false;
    }
    
    // Jeśli tabela ma dane, nie można usuwać kolumn
    if (header.rowsCount > 0) {
        Logger::error("Table: Nie mozna usunac kolumny z tabeli zawierajacej dane");
        return false;
    }
    
    columns.erase(columns.begin() + index);
    header.colsCount--;
    isModified = true;
    
    #ifdef TEST
    Logger::info("Usunieto kolumne: " + columnName);
    #endif
    return true;
}

//  Wyszukuje kolumnę po nazwie i zwraca jej indeks
int Table::findColumnByName(const std::string& name) const {
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i].getName() == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Wyszukuje kolumnę po nazwie i zwraca jej indeks
const Column* Table::getColumn(size_t index) const {
    if (index >= columns.size()) {
        return nullptr;
    }
    return &columns[index];
}

// === INSERT ===
//  Wstawia nowy wiersz do tabeli
int32_t Table::insertRow(const Row& row) {
    // Walidujemy wiersz
    if (!validateRow(row)) {
        Logger::error("Table: Wiersz nie przeszedl walidacji");
        return -1;
    }
    
    // Sprawdzamy limit wierszy
    if (header.rowsCount >= MAX_ROWS_COUNT) {
        Logger::error("Table: Osiagnieto maksymalna liczbe wierszy");
        return -1;
    }
    
    // Tworzymy nowy wiersz z unikalnym ID
    Row newRow = row;
    int32_t newID = generateRowID();
    newRow.setRowID(newID);
    
    // Dodajemy do wektora
    rows.push_back(newRow);
    header.rowsCount++;
    isModified = true;
    
    #ifdef TEST
    Logger::info("Wstawiono wiersz o ID: " + std::to_string(newID));
    #endif
    return newID;
}

// Wstawia nowy wiersz z wektora wartości
int32_t Table::insertRow(const std::vector<CellValue>& values) {
    Row row(0, values); // ID zostanie nadane w insertRow()
    return insertRow(row);
}

// Wstawia wiersz z listy stringów (parsuje typy danych)
int32_t Table::insertRowFromStrings(const std::vector<std::string>& values) {
    try {
        Row row = Row::fromStrings(values, columns);
        return insertRow(row);
    } catch (const std::exception& e) {
        Logger::error("Table: Blad parsowania wiersza: " + std::string(e.what()));
        return -1;
    }
}

// === SELECT ===
// Zwraca wszystkie wiersze tabeli
std::vector<Row> Table::selectAll() const {
    return rows;
}

// --- Wyszukuje wiersz o danym ID
const Row* Table::selectByID(int32_t rowID) const {
    for (const auto& row : rows) {
        if (row.getRowID() == rowID) {
            return &row;
        }
    }
    
    Logger::warn("Table: Nie znaleziono wiersza o ID: " + std::to_string(rowID));
    return nullptr;
}

// Zwraca wybrane kolumny dla danego wiersza (SELECT ...)
Row Table::selectColumns(int32_t rowID, const std::vector<std::string>& columnNames) const {
    const Row* fullRow = selectByID(rowID);
    
    if (!fullRow) {
        return Row(); // Zwracamy pusty wiersz
    }
    
    // Tworzymy nowy wiersz z wybranymi kolumnami
    Row selectedRow(rowID);
    
    for (const auto& colName : columnNames) {
        int colIndex = findColumnByName(colName);
        
        if (colIndex == -1) {
            Logger::warn("Table: Kolumna '" + colName + "' nie istnieje");
            continue;
        }
        
        selectedRow.addCell(fullRow->getCell(colIndex));
    }
    
    return selectedRow;
}

// === UPDATE ===
// Aktualizuje jedną komórkę w tabeli
bool Table::updateCell(int32_t rowID, const std::string& columnName, const CellValue& newValue) {
    // Znajdujemy wiersz
    Row* row = nullptr;
    for (auto& r : rows) {
        if (r.getRowID() == rowID) {
            row = &r;
            break;
        }
    }
    
    if (!row) {
        Logger::error("Table: Nie znaleziono wiersza o ID: " + std::to_string(rowID));
        return false;
    }
    
    // Znajdujemy kolumnę
    int colIndex = findColumnByName(columnName);
    if (colIndex == -1) {
        Logger::error("Table: Kolumna '" + columnName + "' nie istnieje");
        return false;
    }
    
    // TODO: Można dodać szczegółową walidację typu
    
    // Aktualizujemy wartość
    row->setCell(colIndex, newValue);
    isModified = true;
    
    #ifdef TEST
    Logger::info("Zaktualizowano wiersz " + std::to_string(rowID) + ", kolumna " + columnName);
    #endif
    return true;
}

// === DELETE ===
// Usuwa wiersz po ID
bool Table::deleteRow(int32_t rowID) {
    // Znajdujemy i usuwamy wiersz
    auto it = std::find_if(rows.begin(), rows.end(), 
                          [rowID](const Row& r) { return r.getRowID() == rowID; });
    
    if (it == rows.end()) {
        Logger::error("Table: Nie znaleziono wiersza o ID: " + std::to_string(rowID));
        return false;
    }
    
    rows.erase(it);
    header.rowsCount--;
    isModified = true;
    
    #ifdef TEST
    Logger::info("Usunieto wiersz o ID: " + std::to_string(rowID));
    #endif
    return true;
}

// === SERIALIZACJA ===
// Zapisuje strukturę tabeli (nagłówek + kolumny) do pliku binarnego
bool Table::writeStructure() {
    try {
        std::ofstream out(tableFile, std::ios::binary);
        
        if (!out) {
            Logger::error("Table: Nie mozna otworzyc pliku do zapisu: " + tableFile);
            return false;
        }
        
        // Zapisujemy header
        out.write(reinterpret_cast<const char*>(&header), sizeof(TableHeader));
        
        // Zapisujemy kolumny w formacie binarnym
        for (const auto& col : columns) {
            Column::BinaryFormat binCol = col.toBinary();
            out.write(reinterpret_cast<const char*>(&binCol), sizeof(Column::BinaryFormat));
        }
        
        // Wypełniamy resztę pustymi slotami do MAX_COLS_COUNT
        Column::BinaryFormat emptyCol{};
        std::memset(&emptyCol, 0, sizeof(Column::BinaryFormat));
        
        for (size_t i = columns.size(); i < MAX_COLS_COUNT; i++) {
            out.write(reinterpret_cast<const char*>(&emptyCol), sizeof(Column::BinaryFormat));
        }
        
        out.close();
        #ifdef TEST
        Logger::info("Zapisano strukture tabeli: " + tableName);
        #endif
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("Table: Blad zapisu struktury: " + std::string(e.what()));
        return false;
    }
}

// Odczytuje strukturę tabeli z pliku binarnego
bool Table::readStructure() {
    try {
        std::ifstream in(tableFile, std::ios::binary);
        
        if (!in) {
            Logger::error("Table: Nie mozna otworzyc pliku do odczytu: " + tableFile);
            return false;
        }
        
        // Odczytujemy header
        in.read(reinterpret_cast<char*>(&header), sizeof(TableHeader));
        
        // Weryfikujemy sygnaturę
        if (header.signature[0] != 'S' || header.signature[1] != 'T' || 
            header.signature[2] != 'T' || header.signature[3] != 'B') {
            Logger::error("Table: Niepoprawny format pliku (zla sygnatura)");
            return false;
        }
        
        // Odczytujemy kolumny
        columns.clear();
        for (size_t i = 0; i < MAX_COLS_COUNT; i++) {
            Column::BinaryFormat binCol;
            in.read(reinterpret_cast<char*>(&binCol), sizeof(Column::BinaryFormat));
            
            // Dodajemy tylko zajęte sloty
            if (binCol.isOccupied) {
                columns.push_back(Column::fromBinary(binCol));
            }
        }
        
        in.close();
        #ifdef TEST
        Logger::info("Odczytano strukture tabeli: " + tableName);
        #endif
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("Table: Blad odczytu struktury: " + std::string(e.what()));
        return false;
    }
}

// Zapisuje wszystkie dane tabeli (strukturę + wiersze)
bool Table::writeData() {
    try {
        // Najpierw zapisujemy strukturę
        if (!writeStructure()) return false;
        
        
        // Otwieramy plik do dopisywania danych
        std::ofstream out(tableFile, std::ios::binary | std::ios::app);
        
        if (!out) {
            Logger::error("Table: Nie mozna otworzyc pliku do zapisu danych");
            return false;
        }
        
        // Zapisujemy każdy wiersz
        for (const auto& row : rows) {
            // Zapisujemy ID wiersza
            int32_t rowID = row.getRowID();
            out.write(reinterpret_cast<const char*>(&rowID), sizeof(int32_t));
            
            // Zapisujemy każdą komórkę
            for (size_t i = 0; i < row.size(); i++) {
                const CellValue& cell = row.getCell(i);
                // const Column& col = columns[i];
                
                // Zapisujemy według typu
                if (std::holds_alternative<int32_t>(cell)) {
                    int32_t val = std::get<int32_t>(cell);
                    out.write(reinterpret_cast<const char*>(&val), sizeof(int32_t));
                }
                else if (std::holds_alternative<float>(cell)) {
                    float val = std::get<float>(cell);
                    out.write(reinterpret_cast<const char*>(&val), sizeof(float));
                }
                else if (std::holds_alternative<std::string>(cell)) {
                    const std::string& val = std::get<std::string>(cell);
                    uint32_t strLen = static_cast<uint32_t>(val.length() + 1);
                    out.write(reinterpret_cast<const char*>(&strLen), sizeof(uint32_t));
                    out.write(val.c_str(), strLen);
                }
                else if (std::holds_alternative<bool>(cell)) {
                    bool val = std::get<bool>(cell);
                    out.write(reinterpret_cast<const char*>(&val), sizeof(bool));
                }
            }
        }
        
        out.close();
        #ifdef TEST
        Logger::info("Zapisano dane tabeli: " + tableName);
        #endif
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("Table: Blad zapisu danych: " + std::string(e.what()));
        return false;
    }
}

// Odczytuje dane tabeli z pliku binarnego
bool Table::readData() {
    try {
        std::ifstream in(tableFile, std::ios::binary);
        
        if (!in) {
            return false;
        }
        
        // Pomijamy strukturę (header + kolumny)
        size_t structureSize = sizeof(TableHeader) + (MAX_COLS_COUNT * sizeof(Column::BinaryFormat));
        in.seekg(structureSize, std::ios::beg);
        
        // Odczytujemy wiersze
        rows.clear();
        for (uint32_t i = 0; i < header.rowsCount; i++) {
            // Odczytujemy ID wiersza
            int32_t rowID;
            in.read(reinterpret_cast<char*>(&rowID), sizeof(int32_t));
            
            Row row(rowID);
            
            // Odczytujemy każdą komórkę
            for (const auto& col : columns) {
                switch (col.getType()) {
                    case ColumnType::INT: {
                        int32_t val;
                        in.read(reinterpret_cast<char*>(&val), sizeof(int32_t));
                        row.addCell(val);
                        break;
                    }
                    case ColumnType::FLOAT: {
                        float val;
                        in.read(reinterpret_cast<char*>(&val), sizeof(float));
                        row.addCell(val);
                        break;
                    }
                    case ColumnType::VARCHAR: {
                        uint32_t strLen;
                        in.read(reinterpret_cast<char*>(&strLen), sizeof(uint32_t));
                        
                        std::vector<char> buffer(strLen);
                        in.read(buffer.data(), strLen);
                        row.addCell(std::string(buffer.data()));
                        break;
                    }
                    case ColumnType::BOOL: {
                        bool val;
                        in.read(reinterpret_cast<char*>(&val), sizeof(bool));
                        row.addCell(val);
                        break;
                    }
                }
            }
            
            rows.push_back(row);
        }
        
        in.close();
        #ifdef TEST
        Logger::info("Odczytano dane tabeli: " + tableName);
        #endif
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("Table: Blad odczytu danych: " + std::string(e.what()));
        return false;
    }
}

// Zapisuje tabelę do pliku
bool Table::save() {
    return writeData(); // wywołuje writeStructure()
}

// bool Table::save() {
//     std::string tempFile = tableFile + ".tmp";
    
//     try {
//         if (!writeDataToFile(tempFile)) {
//             FileManager::deleteFile(tempFile);
//             return false;
//         }
        
//         FileManager::rename(tempFile, tableFile);
//         isModified = false;
//         return true;
        
//     } catch (const std::exception& e) {
//         Logger::error("Blad zapisu: " + std::string(e.what()));
//         FileManager::deleteFile(tempFile);
//         return false;
//     }
// }

// Wczytuje tabelę z pliku 
bool Table::load() {
    if (!readStructure()) {
        return false;
    }
    return readData();
}

// Zmienia nazwę tabeli (używane przy MODIFYTABLE)
void Table::setName(const std::string& name) {
    if (name.empty()) {
        Logger::error("Table: Nie można ustawić pustej nazwy tabeli");
        return;
    }
    tableName = name;
    isModified = true;
    #ifdef TEST
    Logger::info("Table: Zmieniono nazwę tabeli na: " + name);
    #endif
}

// Zmienia nazwę kolumny po indeksie (używane przy MODIFYCOLUMN)
void Table::renameColumn(int index, const std::string& newName) {
    if (index < 0 || index >= (int)columns.size()) {
        Logger::error("Table: Niepoprawny indeks kolumny: " + std::to_string(index));
        return;
    }
    if (newName.empty()) {
        Logger::error("Table: Nie można ustawić pustej nazwy kolumny");
        return;
    }

    // Sprawdź, czy nowa nazwa już istnieje
    if (findColumnByName(newName) != -1) {
        Logger::error("Table: Kolumna '" + newName + "' już istnieje");
        return;
    }

    columns[index].setName(newName);
    isModified = true;
    // save();  
    #ifdef TEST
    Logger::info("Table: Zmieniono nazwę kolumny: indeks " + std::to_string(index) +  " -> '" + newName + "'");
    #endif
}

// Usuwa kolumnę po indeksie (używane przy DROPCOLUMN)
void Table::dropColumn(int index) {
    if (index < 0 || index >= (int)columns.size()) {
        Logger::error("Table: Niepoprawny indeks kolumny do usunięcia: " + std::to_string(index));
        return;
    }

    // Usuń kolumnę
    columns.erase(columns.begin() + index);
    header.colsCount = columns.size();

    // Usuń dane z każdego wiersza
    for (auto& row : rows) {
        if (index < (int)row.size()) {
            row.erase(index);
        }
    }

    isModified = true;
    // save();  
    #ifdef TEST
    Logger::info("Table: Usunięto kolumnę o indeksie: " + std::to_string(index));
    #endif
}

// Czyści wszystkie dane tabeli (używane przy CLEARTABLE)
void Table::clearData() {
    rows.clear();
    header.rowsCount = 0;
    header.currentRowID = 1; 
    isModified = true;
    // save();  
    #ifdef TEST
    Logger::info("Table: Wyczyszczono wszystkie dane tabeli: " + tableName);
    #endif
}

// === WYŚWIETLANIE ===
//  Wypisuje strukturę kolumn tabel
void Table::printStructure() const {
    std::cout << "\n=== Struktura tabeli: " << tableName << " ===\n";
    std::cout << "Liczba kolumn: " << header.colsCount << "\n\n";
    
    for (size_t i = 0; i < columns.size(); i++) {
        std::cout << "[" << i << "] " << columns[i].toString() << "\n";
    }
    std::cout << std::endl;
}

// Wypisuje zawartość tabeli w formie czytelnej tabelki
void Table::printData(size_t limit) const {
    std::cout << "\n=== Dane tabeli: " << tableName << " ===\n";
    std::cout << "Liczba wierszy: " << header.rowsCount << "\n\n";
    
    // Wyświetlamy nagłówek
    std::cout << std::setw(10) << "Row ID" << " | ";
    for (const auto& col : columns) {
        std::cout << std::setw(15) << col.getName() << " | ";
    }
    std::cout << "\n" << std::string(80, '-') << "\n";
    
    // Wyświetlamy wiersze
    size_t count = 0;
    for (const auto& row : rows) {
        if (limit > 0 && count >= limit) break;
        
        row.print(columns);
        count++;
    }
    
    if (limit > 0 && rows.size() > limit) {
        std::cout << "... (wyswietlono " << limit << " z " << rows.size() << " wierszy)\n";
    }
    
    std::cout << std::endl;
}

// Wypisuje statystyki tabeli
void Table::printStats() const {
    std::cout << "\n=== Statystyki tabeli: " << tableName << " ===\n";
    std::cout << "Plik: " << tableFile << "\n";
    std::cout << "Kolumny: " << header.colsCount << " / " << MAX_COLS_COUNT << "\n";
    std::cout << "Wiersze: " << header.rowsCount << " / " << MAX_ROWS_COUNT << "\n";
    std::cout << "Nastepny ID: " << header.currentRowID << "\n";
    std::cout << "Zmodyfikowana: " << (isModified ? "TAK" : "NIE") << "\n";
    std::cout << std::endl;
}

// === METODY POMOCNICZE ===
// Wypisuje statystyki tabeli
bool Table::validateRow(const Row& row) const {
    // Sprawdzamy czy liczba komórek zgadza się z liczbą kolumn
    if (row.size() != columns.size()) {
        Logger::error("Table: Liczba wartosci (" + std::to_string(row.size()) + 
                     ") nie zgadza sie z liczba kolumn (" + std::to_string(columns.size()) + ")");
        return false;
    }
    
    // Sprawdzamy każdą komórkę
    for (size_t i = 0; i < row.size(); i++) {
        if (!row.validateCellType(i, columns[i])) {
            Logger::error("Table: Niepoprawny typ w kolumnie " + columns[i].getName());
            return false;
        }
    }
    
    return true;
}

// Sprawdza, czy są jeszcze wolne sloty na nowe wiersze
int32_t Table::generateRowID() {
    return ++header.currentRowID;
}