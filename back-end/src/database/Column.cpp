#include "../../include/database/Column.hpp"
#include "../../include/Logger.hpp"
#include <cstring>
#include <stdexcept>
#include <sstream>

// === KONSTRUKTORY ===
// Tworzy pustą kolumnę (slot wolny w tabeli)
Column::Column(): name(""), type(ColumnType::INT), size(0), isPrimaryKey(false), nullable(true), isOccupied(false) {}

// Konstruktor z parametrami — tworzy kolumnę o określonym typie i nazwie
Column::Column(const std::string& name, ColumnType type, uint32_t size, bool isPrimaryKey, bool nullable)
    : name(name), type(type), size(size), isPrimaryKey(isPrimaryKey), nullable(nullable), isOccupied(true) {
    
    // Walidacja nazwy
    if (!isValidName()) {
        throw std::invalid_argument("Column: Niepoprawna nazwa kolumny");
    }
    
    // Dla typów nie-VARCHAR ignorujemy rozmiar i ustawiamy domyślny
    if (type != ColumnType::VARCHAR) {
        this->size = getDefaultSize(type);
    } else {
        // Dla VARCHAR rozmiar musi być > 0
        if (size == 0) {
            throw std::invalid_argument("Column: VARCHAR wymaga rozmiaru > 0");
        }
    }
    
    Logger::info("Utworzono kolumne: " + name + " (" + typeToString() + ")");
}

// === WALIDACJA ===
// Sprawdza, czy nazwa kolumny jest poprawna (niepusta, niezbyt długa)
bool Column::isValidName() const {
    // Nazwa nie może być pusta
    if (name.empty()) {
        return false;
    }
    
    // Nazwa nie może przekraczać MAX_COL_NAME
    if (name.length() >= MAX_COL_NAME) {
        Logger::error("Column: Nazwa kolumny za dluga (max " + std::to_string(MAX_COL_NAME) + ")");
        return false;
    }
    
    // TODO: Można dodać sprawdzanie czy nazwa zawiera tylko dozwolone znaki
    // (litery, cyfry, podkreślnik, bez spacji i znaków specjalnych)
    
    return true;
}

// Sprawdza, czy podana wartość pasuje do typu kolumny
bool Column::validateValue(const std::string& value) const {
    try {
        switch (type) {
            case ColumnType::INT:
                // Próbujemy sparsować jako int
                std::stoi(value);
                return true;
                
            case ColumnType::FLOAT:
                // Próbujemy sparsować jako float
                std::stof(value);
                return true;
                
            case ColumnType::VARCHAR:
                // Sprawdzamy długość stringa
                if (value.length() > size) {
                    Logger::warn("Column: Wartosc VARCHAR za dluga dla kolumny " + name);
                    return false;
                }
                return true;
                
            case ColumnType::BOOL:
                // Akceptujemy: "true", "false", "1", "0"
                if (value == "true" || value == "false" || 
                    value == "1" || value == "0") {
                    return true;
                }
                Logger::warn("Column: Niepoprawna wartosc BOOL dla kolumny " + name);
                return false;
                
            default:
                return false;
        }
    } catch (const std::exception& e) {
        Logger::warn("Column: Blad walidacji wartosci: " + std::string(e.what()));
        return false;
    }
}

// === KONWERSJE ===
// Zwraca nazwę typu kolumny jako tekst
std::string Column::typeToString() const {
    switch (type) {
        case ColumnType::INT:     return "INT";
        case ColumnType::FLOAT:   return "FLOAT";
        case ColumnType::VARCHAR: return "VARCHAR(" + std::to_string(size) + ")";
        case ColumnType::BOOL:    return "BOOL";
        default:                  return "UNKNOWN";
    }
}

// Konwertuje typ kolumny (enum) na wartość liczbową (uint8_t)
uint8_t Column::typeToUint8() const {
    return static_cast<uint8_t>(type);
}

// Zwraca domyślny rozmiar w bajtach dla danego typu
uint32_t Column::getDefaultSize(ColumnType type) {
    switch (type) {
        case ColumnType::INT:     return sizeof(int32_t);   // 4 bajty
        case ColumnType::FLOAT:   return sizeof(float);     // 4 bajty
        case ColumnType::BOOL:    return sizeof(bool);      // 1 bajt
        case ColumnType::VARCHAR: return 0;                 // Zmienny
        default:                  return 0;
    }
}

// === SERIALIZACJA ===
// Konwertuje kolumnę na format binarny (do zapisu w pliku)
Column::BinaryFormat Column::toBinary() const {
    BinaryFormat binary;
    
    // Wypełniamy strukturę zerami (bezpieczne)
    std::memset(&binary, 0, sizeof(BinaryFormat));
    
    // Ustawiamy pola
    binary.isOccupied = isOccupied ? 1 : 0;
    binary.size = size;
    binary.datatype = typeToUint8();
    
    // Kopiujemy nazwę ( ze sprawdzeniem długości)
    std::strncpy(binary.name, name.c_str(), MAX_COL_NAME - 1);
    binary.name[MAX_COL_NAME - 1] = '\0';
    
    return binary;
}

// Tworzy obiekt Column z danych binarnych
Column Column::fromBinary(const BinaryFormat& binary) {
    // Tworzymy pusty obiekt
    Column col;
    
    // Odczytujemy pola
    col.isOccupied = (binary.isOccupied != 0);
    col.size = binary.size;
    col.type = static_cast<ColumnType>(binary.datatype);
    
    // Odczytujemy nazwę (bezpiecznie)
    col.name = std::string(binary.name);
    
    // Ustawiamy domyślne wartości dla pól, których nie ma w BinaryFormat
    col.isPrimaryKey = false;
    col.nullable = true;
    
    return col;
}

// === WYŚWIETLANIE ===
// Zwraca tekstowy opis kolumny
std::string Column::toString() const {
    if (!isOccupied) {
        return "[EMPTY SLOT]";
    }
    
    std::stringstream ss;
    
    // Nazwa i typ
    ss << name << " " << typeToString();
    
    // Dodatkowe atrybuty
    if (isPrimaryKey) {
        ss << " PRIMARY KEY";
    }
    
    if (!nullable) {
        ss << " NOT NULL";
    }
    
    return ss.str();
}