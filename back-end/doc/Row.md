# Row

`Row` to klasa reprezentująca **jeden wiersz danych** w tabeli **StupidDB**.

---

## Główne funkcje

| Funkcja | Co robi? | Przykład użycia |
|--------|----------|-----------------|
| `Row(1, {1, "Anna", 20, true})`   | Tworzy wiersz z danymi            | ```Row r(1, {1, "Anna", 20, true});``` |
| `r.addCell("Bartek")`             | Dodaje wartość na końcu           | ```r.addCell("Bartek");``` |
| `r[1] = "Kasia"`                  | Zmienia wartość (operator `[]`)   | ```r[1] = "Kasia";``` |
| `r.getCell(0)`                    | Pobiera wartość                   | ```CellValue val = r.getCell(0);``` |
| `r.toString()`                    | Wiersz jako tekst                 | ```std::cout << r.toString();``` |
| `r.print(columns)`                | Wyświetla wiersz w tabeli         | ```r.print(columns);``` |

---

## Typy wartości w komórce (`CellValue`)

| Typ | Przykład |
|-----|---------|
| `int32_t`     | `42`    |
| `float`       | `3.14`  |
| `std::string` | `"Jan"` |
| `bool`        | `true`  |

---

## Przykład: Praca z wierszem

```cpp
#include "Row.h"
#include "Column.h"

// Definicje kolumn
std::vector<Column> columns = {
    Column("id", ColumnType::INT, 0, true, false),
    Column("name", ColumnType::VARCHAR, 50),
    Column("age", ColumnType::INT),
    Column("active", ColumnType::BOOL)
};

// Nowy wiersz
Row row(5);
row.addCell(5);           // id
row.addCell("Ola");       // name
row.addCell(19);          // age
row.addCell(true);        // active

// Wyświetl
row.print(columns);