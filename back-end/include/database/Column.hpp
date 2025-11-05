#pragma once

#include <string>
#include <cstdint>

#define MAX_COL_NAME 512 // Maksymalna długość nazwy kolumny 

enum class ColumnType : uint8_t {
    INT = 0,      // 32-bitowa liczba całkowita
    FLOAT = 1,    // 32-bitowa liczba zmiennoprzecinkowa
    VARCHAR = 2,  // Tekst o zmiennej długości
    BOOL = 3      // Wartość logiczna (true/false)
};

class Column {
private:
    std::string name;         // Nazwa kolumny
    ColumnType type;          // Typ danych
    uint32_t size;            // Rozmiar (dla VARCHAR - max długość, dla innych - 0)
    bool isPrimaryKey;        // Czy kolumna jest kluczem głównym
    bool nullable;            // Czy kolumna może być NULL
    bool isOccupied;          // Czy slot kolumny jest zajęty 

public:
    // === KONSTRUKTORY ===
    
    // Konstruktor pusty - tworzy pustą kolumnę
    Column();
    
    // Tworzy kolumnę z pełną definicją
    Column(const std::string& name, ColumnType type, uint32_t size = 0, bool isPrimaryKey = false, bool nullable = true);
    
    // === GETTERY ===
    
    std::string getName()   const { return name; }
    ColumnType getType()    const { return type; }
    uint32_t getSize()      const { return size; }
    bool getIsPrimaryKey()  const { return isPrimaryKey; }
    bool isNullable()       const { return nullable; }
    bool getIsOccupied()    const { return isOccupied; }
    
    // === SETTERY ===
    void setOccupied(bool occupied) { isOccupied = occupied; }
    void setName(const std::string& newName) { name = newName; }
    
    // === WALIDACJA ===
    
    // Sprawdza czy nazwa kolumny jest poprawna
    bool isValidName() const;
    // Sprawdza czy wartość typu string pasuje do typu kolumny
    bool validateValue(const std::string& value) const;
    
    // === KONWERSJE ===
    
    // Konwertuje typ kolumny na string
    std::string typeToString() const;
    // Konwertuje enum ColumnType na uint8_t (dla zapisu binarnego)
    uint8_t typeToUint8() const;
    // Zwraca domyślny rozmiar dla danego typu
    static uint32_t getDefaultSize(ColumnType type);
    
    // === SERIALIZACJA ) ===
    
    // Struktura binarna używana do zapisu/odczytu z pliku binarnego
    struct BinaryFormat {
        uint8_t isOccupied;
        char name[MAX_COL_NAME];
        uint32_t size;
        uint8_t datatype;
    };
    
    // Konwertuje obiekt Column do formatu binarnego
    BinaryFormat toBinary() const;
    // Tworzy obiekt Column z formatu binarnego
    static Column fromBinary(const BinaryFormat& binary);
    
    // === WYŚWIETLANIE ===
    // Zwraca pełną reprezentację kolumny jako string
    std::string toString() const;
};