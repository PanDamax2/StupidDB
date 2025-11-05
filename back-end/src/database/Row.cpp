#include "../../include/database/Row.hpp"
#include "../../include/Logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

// === KONSTRUKTORY ===
// Konstruktor pustego wiersza (bez danych, ID = -1)
Row::Row() : rowID(-1) {}

// Konstruktor wiersza z określonym ID, ale bez danych
Row::Row(int32_t rowID) : rowID(rowID) {}

// Konstruktor wiersza z określonym ID i danymi
Row::Row(int32_t rowID, const std::vector<CellValue>& cells) : rowID(rowID), cells(cells) {}

// === OPERACJE NA KOMÓRKACH ===
// Dodaje nową komórkę (wartość) do wiersza
void Row::addCell(const CellValue& value) {
    cells.push_back(value);
}

// Ustawia wartość istniejącej komórki
void Row::setCell(size_t index, const CellValue& value) {
    if (index >= cells.size()) {
        throw std::out_of_range("Row: Indeks poza zakresem");
    }
    cells[index] = value;
}

// Usuwa komórkę o podanym indeksie
void Row::erase(size_t index) {
    if (index < cells.size()) {
        cells.erase(cells.begin() + index);
    }
}

// Zwraca wartość komórki o podanym indeksie
CellValue Row::getCell(size_t index) const {
    if (index >= cells.size()) {
        throw std::out_of_range("Row: Indeks poza zakresem");
    }
    return cells[index];
}

// Czyści wszystkie komórki w wierszu
void Row::clear() {
    cells.clear();
}

// === WALIDACJA ===
// Sprawdza czy wiersz jest zgodny z kolumnami tabeli
bool Row::validateAgainstColumns(const std::vector<Column>& columns) const {
    // Liczba komórek musi się zgadzać z liczbą kolumn
    if (cells.size() != columns.size()) {
        Logger::warn("Row: Liczba wartosci (" + std::to_string(cells.size()) + ") nie zgadza sie z liczba kolumn (" + std::to_string(columns.size()) + ")");
        return false;
    }
    
    // Sprawdzamy każdą komórkę
    for (size_t i = 0; i < cells.size(); i++) {
        if (!validateCellType(i, columns[i])) {
            Logger::warn("Row: Niepoprawny typ wartosci w kolumnie " + columns[i].getName());
            return false;
        }
    }
    
    return true;
}

// Sprawdza, czy wartość komórki pasuje do typu danej kolumny
bool Row::validateCellType(size_t index, const Column& column) const {
    if (index >= cells.size()) {
        return false;
    }
    
    const CellValue& value = cells[index];
    ColumnType expectedType = column.getType();
    
    // Sprawdzamy czy typ wartości zgadza się z typem kolumny
    switch (expectedType) {
        case ColumnType::INT:
            return std::holds_alternative<int32_t>(value);
            
        case ColumnType::FLOAT:
            return std::holds_alternative<float>(value);
            
        case ColumnType::VARCHAR:
            if (std::holds_alternative<std::string>(value)) {
                // Sprawdzamy długość dla VARCHAR
                const std::string& str = std::get<std::string>(value);
                return str.length() <= column.getSize();
            }
            return false;
            
        case ColumnType::BOOL:
            return std::holds_alternative<bool>(value);
            
        default:
            return false;
    }
}

// === KONWERSJE ===
// Tworzy wiersz na podstawie wartości tekstowych i typów kolumn
Row Row::fromStrings(const std::vector<std::string>& values, const std::vector<Column>& columns) {
    
    if (values.size() != columns.size()) {
        throw std::invalid_argument("Row: Liczba wartosci nie zgadza sie z liczba kolumn");
    }
    
    Row row;
    
    // Parsujemy każdą wartość według typu kolumny
    for (size_t i = 0; i < values.size(); i++) {
        try {
            CellValue cellValue = parseValue(values[i], columns[i].getType());
            row.addCell(cellValue);
        } catch (const std::exception& e) {
            Logger::error("Row: Blad parsowania wartosci '" + values[i] + "' dla kolumny " + columns[i].getName() + ": " + e.what());
            throw;
        }
    }
    
    return row;
}

// Zamienia wartość komórki na tekst
std::string Row::cellToString(const CellValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int32_t>) {
            return std::to_string(arg);
        }
        else if constexpr (std::is_same_v<T, float>) {
            // Formatujemy float z 2 miejscami po przecinku
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << arg;
            return ss.str();
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }
        else {
            return "UNKNOWN";
        }
    }, value);
}

// Konwertuje tekst na odpowiedni typ danych dla komórki
CellValue Row::parseValue(const std::string& value, ColumnType type) {
    try {
        switch (type) {
            case ColumnType::INT:       return static_cast<int32_t>(std::stoi(value)); 
            case ColumnType::FLOAT:     return std::stof(value);
            case ColumnType::VARCHAR:   return value;
            case ColumnType::BOOL:
                // Akceptujemy różne formaty
                if (value == "true" || value == "1" || value == "TRUE")         return true;
                else if (value == "false" || value == "0" || value == "FALSE")  return false;
                throw std::invalid_argument("Niepoprawna wartosc BOOL: " + value);
                
            default:
                throw std::invalid_argument("Nieobslugiwany typ kolumny");
        }
    } catch (const std::exception& e) {
        throw std::invalid_argument("Blad parsowania wartosci: " + std::string(e.what()));
    }
}

// === WYŚWIETLANIE ===
// Zwraca wiersz w formie tekstowej
std::string Row::toString() const {
    std::stringstream ss;
    ss << "Row[" << rowID << "]: ";
    
    for (size_t i = 0; i < cells.size(); i++) {
        if (i > 0) ss << " | ";
        ss << cellToString(cells[i]);
    }
    
    return ss.str();
}

// Wyświetla wiersz w konsoli 
void Row::print(const std::vector<Column>& columns) const {
    // Wyświetlamy ID wiersza
    std::cout << "Row " << std::setw(5) << rowID << " | ";
    
    // Wyświetlamy wartości komórek
    for (size_t i = 0; i < cells.size() && i < columns.size(); i++) {
        std::string value = cellToString(cells[i]);
        
        // Formatujemy z wyrównaniem
        std::cout << std::setw(15) << value;
        
        if (i < cells.size() - 1) {
            std::cout << " | ";
        }
    }
    
    std::cout << std::endl;
}

// === OPERATORY ===
// Operator [] do bezpośredniego dostępu do komórki
CellValue& Row::operator[](size_t index) {
    if (index >= cells.size()) {
        throw std::out_of_range("Row: Indeks poza zakresem");
    }
    return cells[index];
}

// Operator [] w wersji tylko do odczytu
const CellValue& Row::operator[](size_t index) const {
    if (index >= cells.size()) {
        throw std::out_of_range("Row: Indeks poza zakresem");
    }
    return cells[index];
}