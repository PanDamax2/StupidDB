# Column 
`Column` to klasa reprezentująca **jedną kolumnę w tabeli** w **StupidDB**.

---

## Główne funkcje

| Funkcja | Co robi? | Przykład użycia |
|--------|----------|-----------------|
| `Column("id", ColumnType::INT, 0, true)`  | Tworzy kolumnę                        | ```Column id("id", ColumnType::INT, 0, true, false);``` |
| `col.getName()`                           | Zwraca nazwę                          | ```std::string name = col.getName();``` |
| `col.getType()`                           | Zwraca typ (`INT`, `VARCHAR` itp.)    | ```ColumnType t = col.getType();``` |
| `col.isNullable()`                        | Czy może być `NULL`?                  | ```if (col.isNullable()) { ... }``` |
| `col.getIsPrimaryKey()`                   | Czy klucz główny?                     | ```if (col.getIsPrimaryKey()) { ... }``` |
| `col.toString()`                          | Pełna definicja jako tekst            | ```std::cout << col.toString();``` |

---

## Typy danych (`ColumnType`)

| Typ | Opis | Rozmiar domyślny |
|-----|------|------------------|
| `INT`             | Liczba całkowita | 4 bajty |
| `FLOAT`           | Liczba zmiennoprzecinkowa | 4 bajty |
| `VARCHAR(size)`   | Tekst (max `size` znaków) | `size` |
| `BOOL`            | `true` / `false` | 1 bajt |

---

## Przykład: Tworzenie kolumn

```cpp
#include "Column.h"

// Kolumna: id (klucz główny, nie NULL)
Column id("id", ColumnType::INT, 0, true, false);

// Kolumna: imię (tekst do 50 znaków)
Column name("name", ColumnType::VARCHAR, 50);

// Kolumna: wiek (liczba)
Column age("age", ColumnType::INT);

// Kolumna: aktywny (tak/nie)
Column active("active", ColumnType::BOOL);

std::cout << id.toString() << "\n";     // id INT PRIMARY KEY NOT NULL
std::cout << name.toString() << "\n";   // name VARCHAR(50)