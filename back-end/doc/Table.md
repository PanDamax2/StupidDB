# Table
`Table` to główna klasa reprezentująca **tabelę w bazie danych** w **StupidDB**.

---

## Główne funkcje

| Funkcja | Co robi? | Przykład użycia |
|--------|----------|-----------------|
| `Table("users", "db/school/users.table")` | Tworzy nową tabelę                | ```Table t("users", "db/school/users.table");``` |
| `t.init()`                                | Odczytuje lub tworzy plik tabeli  | ```t.init();``` |
| `t.createColumn(Column("id", ColumnType::INT, 0, true))` | Dodaje kolumnę     | ```t.createColumn(Column("id", ColumnType::INT, 0, true));``` |
| `t.insertRow({"Jan", 25, "true"})`        | Wstawia wiersz                    | ```t.insertRow({"Jan", 25, "true"});``` |
| `t.selectAll()`                           | Zwraca wszystkie wiersze          | ```auto rows = t.selectAll();``` |
| `t.printData()`                           | Wyświetla tabelę w konsoli        | ```t.printData();``` |
| `t.save()`                                | Zapisuje tabelę do pliku          | ```t.save();``` |
| `t.deleteRow(5)`                          | Usuwa wiersz po ID                | ```t.deleteRow(5);``` |
| `t.printStructure()`                      | Pokazuje kolumny                  | ```t.printStructure();``` |

---

## Co robi `Table`?

| Zadanie | Jak to robi? |
|--------|-------------|
| **Tworzy plik `.table`**  | Zapisuje nagłówek + kolumny + dane |
| **Dodaje kolumny**        | `createColumn()` |
| **Dodaje wiersze**        | `insertRow()` |
| **Czyta dane**            | `selectAll()`, `selectByID()` |
| **Usuwa wiersze**         | `deleteRow()` |
| **Waliduje dane**         | Sprawdza typy, NULL, długość |
| **Zapisuje zmiany**       | `save()` → `writeStructure()` + `writeData()` |

---

## Przykład: Tworzenie tabeli "users"

```cpp
#include "Table.h"
#include "Column.h"
#include "Logger.h"

int main() {
    Table users("users", "db/school/users.table");

    if (!users.init()) {
        Logger::error("Nie udalo sie zaladowac tabeli");
        return 1;
    }

    // Dodaj kolumny (tylko raz!)
    if (users.getColsCount() == 0) {
        users.createColumn(Column("id", ColumnType::INT, 0, true, false));     // PK
        users.createColumn(Column("name", ColumnType::VARCHAR, 50));           // tekst
        users.createColumn(Column("age", ColumnType::INT));
        users.createColumn(Column("active", ColumnType::BOOL));
        users.save();
        Logger::info("Struktura tabeli utworzona");
    }

    // Wstaw dane
    users.insertRow({1, "Anna", 20, true});
    users.insertRow({2, "Bartek", 22, false});
    users.save();

    // Pokaż dane
    users.printStructure();
    users.printData();

    return 0;
}