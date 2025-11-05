#include "Test.h"
#include "../include/FileManager.hpp"
#include <iostream>
#include <string>

void testFileManager() {
    std::cout << "\n=== TESTUJEMY FileManager  ===\n";

    const std::string base = "./test";
    const std::string dir  = base + "/test_db";
    const std::string sub  = dir  + "/subdir";
    const std::string file = dir  + "/data.txt";
    const std::string bin  = dir  + "/data.bin";

    // === 1. createDirectory (z podkatalogami) ===
    std::cout << "1. createDirectory\n";
    if (FileManager::createDirectory(sub)) {
        std::cout << "  [OK] Katalog utworzony: " << sub << "\n";
    } else {
        std::cout << "  [FAIL] Nie utworzono katalogu!\n";
        return;
    }

    // === 2. exists + isDirectory ===
    std::cout << "2. exists + isDirectory\n";
    if (FileManager::exists(dir) && FileManager::isDirectory(dir)) {
        std::cout << "  [OK] exists() i isDirectory() dzialaja\n";
    } else {
        std::cout << "  [FAIL] Blad sprawdzania istnienia!\n";
    }

    // === 3. writeFile + readFile ===
    std::cout << "3. writeFile + readFile\n";
    if (FileManager::writeFile(file, "Test 123\nLinia 2")) {
        std::cout << "  [OK] Plik zapisany\n";
    } else {
        std::cout << "  [FAIL] Blad zapisu!\n";
    }

    std::string content = FileManager::readFile(file);
    if (content == "Test 123\nLinia 2") {
        std::cout << "  [OK] Odczyt poprawny\n";
    } else {
        std::cout << "  [FAIL] Odczyt bledny! Otrzymano: '" << content << "'\n";
    }

    // === 4. listFiles + listDirectories ===
    std::cout << "4. listFiles + listDirectories\n";
    auto files = FileManager::listFiles(dir);
    auto dirs  = FileManager::listDirectories(dir);

    if (files.size() == 1 && files[0] == "data.txt") {
        std::cout << "  [OK] listFiles() dziala\n";
    } else {
        std::cout << "  [FAIL] listFiles() bledny!\n";
    }
    if (dirs.size() == 1 && dirs[0] == "subdir") {
        std::cout << "  [OK] listDirectories() dziala\n";
    } else {
        std::cout << "  [FAIL] listDirectories() bledny!\n";
    }

    // === 5. writeBinary + readBinary ===
    std::cout << "5. writeBinary + readBinary\n";
    int value = 12345;
    if (FileManager::writeBinary(bin, &value, sizeof(value))) {
        std::cout << "  [OK] Dane binarne zapisane\n";
    }

    int read_value = 0;
    if (FileManager::readBinary(bin, &read_value, sizeof(read_value)) && read_value == 12345) {
        std::cout << "  [OK] Dane binarne wczytane poprawnie\n";
    } else {
        std::cout << "  [FAIL] Dane binarne uszkodzone!\n";
    }

    // === 6. getFileSize ===
    std::cout << "6. getFileSize\n";
    size_t size = FileManager::getFileSize(file);
    if (size == 16) {  // "Test 123\nLinia 2" = 16 bajtow
        std::cout << "  [OK] Rozmiar pliku poprawny\n";
    } else {
        std::cout << "  [FAIL] Zly rozmiar: " << size << "\n";
    }

    // === 7. deleteFile ===
    std::cout << "7. deleteFile\n";
    if (FileManager::deleteFile(file) && FileManager::deleteFile(bin)) {
        std::cout << "  [OK] Pliki usuniete\n";
    } else {
        std::cout << "  [FAIL] Nie usunieto plikow!\n";
    }

    // === 8. removeDirectory  ===
    std::cout << "8. removeDirectory\n";
    if (FileManager::removeDirectory(base + "/test_db")) {
        std::cout << "  [OK] Katalog test_db usuniety\n";
    } else {
        std::cout << "  [FAIL] Nie usunieto test_db!\n";
    }

    // === 9. BŁĄD: nieistniejący plik ===
    std::cout << "9. Blad: odczyt nieistniejacego pliku\n";
    std::string empty = FileManager::readFile("./test/brak.txt");
    if (empty.empty()) {
        std::cout << "  [OK] readFile() zwrocil pusty string\n";
    } else {
        std::cout << "  [FAIL] readFile() nie powinien nic zwrocic!\n";
    }

    // === 10. BŁĄD: usuwanie nieistniejącego ===
    std::cout << "10. Blad: usuwanie nieistniejacego pliku\n";
    if (!FileManager::deleteFile("./test/brak.txt")) {
        std::cout << "  [OK] deleteFile() zwrocil false\n";
    } else {
        std::cout << "  [FAIL] deleteFile() nie powinien usunac!\n";
    }

    std::cout << "=== KONIEC TESTOW FileManager ===\n\n";
}