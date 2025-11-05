// // === 1. Utwórz strukturę katalogów ===
// std::string basePath = "db";
// std::string dbName = "example_db";
// std::string dbPath = basePath + "/" + dbName;
// std::string tablesPath = dbPath + "/tables";

// // Utwórz katalog "db"
// if (!FileManager::exists(basePath)) {
//     FileManager::createDirectory(basePath);
//     Logger::info("Utworzono katalog: " + basePath);
// }

// // Utwórz katalog "db/example_db"
// if (!FileManager::exists(dbPath)) {
//     FileManager::createDirectory(dbPath);
//     Logger::info("Utworzono katalog: " + dbPath);
// }

// // Utwórz katalog "db/example_db/tables"
// if (!FileManager::exists(tablesPath)) {
//     FileManager::createDirectory(tablesPath);
//     Logger::info("Utworzono katalog: " + tablesPath);
// }

// // Utwórz plik metadanych bazy "db/example_db/db.stdb"
// std::string dbFile = dbPath + "/db.stdb";
// if (!FileManager::exists(dbFile)) {
//     std::ofstream meta(dbFile, std::ios::binary);
//     if (meta) {
//         std::string header = "STUPID_DB_V1";  // prosty nagłówek
//         meta.write(header.c_str(), header.size());
//         meta.close();
//         Logger::info("Utworzono plik metadanych: " + dbFile);
//     }
// }

// // === 2. Tworzymy tabelę ===
// Table users("users", tablesPath + "/users.sttb");

// // === 3. Inicjalizacja ===
// if (!users.init()) {
//     Logger::error("Nie udało się załadować tabeli!");
//     return 1;
// }

// // === 4. Dodaj kolumny (tylko raz!) ===
// if (users.getColsCount() == 0) {
//     users.createColumn(Column("id", ColumnType::INT, 0, true, false));
//     users.createColumn(Column("name", ColumnType::VARCHAR, 50));
//     users.createColumn(Column("age", ColumnType::INT));
//     Logger::info("Struktura tabeli utworzona");
// }

// // === 5. Wstaw dane ===
// users.insertRow({1, "Anna", 20});
// users.insertRow({2, "Bartek", 22});

// // === 6. Zapisz ===
// if (users.save()) {
//     Logger::info("Tabela zapisana!");
// }

// // === 7. Pokaż dane ===
// users.printStructure();
// users.printData();