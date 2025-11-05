#pragma once

#include <vector>
#include <variant>
#include <string>
#include <cstdint>
#include "Column.hpp"


using CellValue = std::variant<int32_t, float, std::string, bool>;


class Row {
private:
    int32_t rowID;                     // ID wiersza 
    std::vector<CellValue> cells;      // Wartości komórek

public:
    // === KONSTRUKTORY ===
    
    // Konstruktor domyślny - tworzy pusty wiersz
    Row();
    
    // Tworzy wiersz o danym ID
    explicit Row(int32_t rowID);
    
    // Tworzy wiersz z wartościami
    Row(int32_t rowID, const std::vector<CellValue>& cells);
    
    // === GETTERY / SETTERY ===
    
    int32_t getRowID() const { return rowID; }
    void setRowID(int32_t id) { rowID = id; }
    
    const std::vector<CellValue>& getCells() const { return cells; }
    std::vector<CellValue>& getCells() { return cells; }
    
    size_t size() const { return cells.size(); }
    bool isEmpty() const { return cells.empty(); }
    
    // === OPERACJE NA KOMÓRKACH ===
    
    // Dodaje wartość na końcu wiersza
    void addCell(const CellValue& value);
    // Ustawia wartość w danej pozycji
    void setCell(size_t index, const CellValue& value);
    // Usuwa komórkę o danym indeksie
    void erase(size_t index);  
    // Pobiera wartość z danej pozycji
    CellValue getCell(size_t index) const;
    // Czyści wszystkie komórki
    void clear();
    
    // === WALIDACJA ===
    
    // Sprawdza czy wiersz pasuje do definicji kolumn
    bool validateAgainstColumns(const std::vector<Column>& columns) const;
    // Sprawdza czy wartość w komórce pasuje do typu kolumny
    bool validateCellType(size_t index, const Column& column) const;
    
    // === KONWERSJE ===
    
    // Tworzy wiersz z wektora stringów 
    static Row fromStrings(const std::vector<std::string>& values, const std::vector<Column>& columns);
    // Konwertuje wartość komórki na string (do wyświetlania)
    static std::string cellToString(const CellValue& value);
    // Parsuje string do wartości odpowiedniego typu
    static CellValue parseValue(const std::string& value, ColumnType type);
    
    // === WYŚWIETLANIE ===
    
    // Zwraca reprezentację wiersza jako string
    std::string toString() const;
    // Wyświetla wiersz w formie tabeli
    void print(const std::vector<Column>& columns) const;
    
    // === OPERATORY ===
    
    // Operator [] - dostęp do komórki
    CellValue& operator[](size_t index);
    const CellValue& operator[](size_t index) const;
};