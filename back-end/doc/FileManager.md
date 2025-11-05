# FileManager

`FileManager` to klasa pomocnicza do pracy z plikami i katalogami w **StupidDB**.

---

## Główne funkcje

| Funkcja | Co robi? | Przykład użycia |
|--------|----------|-----------------|
| `removeDirectory("db/moja")`                          | Usuwa katalog **i całą jego zawartość**   | ```FileManager::removeDirectory("db/moja");``` → usuwa całą bazę |
| `exists("db/moja")`                                   | Sprawdza, czy plik lub katalog istnieje   | ```if (!FileManager::exists("db/moja")) { ... }``` |
| `isDirectory("db/moja")`                              | Czy dany ścieżka to katalog?              | ```if (FileManager::isDirectory("db")) { ... }``` |
| `writeFile("db/moja/db.stdb", "tekst")`               | Zapisuje tekst do pliku                   | ```FileManager::writeFile("db/moja/db.stdb", "Nazwa: moja\n");``` |
| `readFile("db/moja/db.stdb")`                         | Czyta cały plik jako `std::string`        | ```std::string data = FileManager::readFile("db/moja/db.stdb");``` |
| `writeBinary("db/moja/data.bin", &obj, sizeof(obj))`  | Zapisuje dane binarne (np. struktury)     | ```User u; FileManager::writeBinary("plik.bin", &u, sizeof(u));``` |
| `readBinary("db/moja/data.bin", &obj, sizeof(obj))`   | Czyta dane binarne do pamięci             | ```User u; FileManager::readBinary("plik.bin", &u, sizeof(u));``` |
| `getFileSize("db/moja/db.stdb")`                      | Zwraca rozmiar pliku w bajtach            | ```size_t size = FileManager::getFileSize("plik.txt");``` |
| `listFiles("db/moja/tables")`                         | Zwraca listę **nazw plików** w katalogu   | ```auto files = FileManager::listFiles("db/moja/tables");``` |
| `listDirectories("db")`                               | Zwraca listę **nazw podkatalogów**        | ```auto dbs = FileManager::listDirectories("db");``` |
| `deleteFile("db/moja/tabela.table")`                  | Usuwa pojedynczy plik                     | ```FileManager::deleteFile("stara_tabela.table");``` |
| `createDirectory("db/moja")`                          | Tworzy katalog (i wszystkie nadrzędne)    | ```FileManager::createDirectory("db/moja/tables");``` → tworzy `db/moja/tables/` |

---




## Przykład: Tworzenie bazy danych

```cpp
std::string path = "db/school_db";
if (!FileManager::exists(path)) {
    FileManager::createDirectory(path + "/tables");
    FileManager::writeFile(path + "/db.stdb", "Nazwa: school_db\n");
    Logger::info("Baza utworzona!");
}