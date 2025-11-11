#include "../../include/FileManager.hpp"
#include <fstream>
#include <sstream>

// Tworzy katalog
bool FileManager::createDirectory(const std::string& path) {
    try {
        if (fs::create_directories(path)) {
            Logger::info("Utworzono katalog: " + path);
            return true;
        } else if (fs::exists(path)) {
            Logger::info("Katalog juz istnieje: " + path);
            return true;
        }

        return false;
    } catch (const std::exception& e) {
        Logger::error("Blad tworzenia katalogu '" + path + "': " + e.what());
        return false;
    }
}

// Zmienia nazwe
bool FileManager::rename(const std::string& oldPath, const std::string& newPath) {
    try {
        if (!fs::exists(oldPath)) {
            Logger::warn("Nie mozna zmienic nazwy: sciezka nie istnieje: " + oldPath);
            return false;
        }

        if (fs::exists(newPath)) {
            Logger::warn("Nie mozna zmienic nazwy: nowa sciezka juz istnieje: " + newPath);
            return false;
        }

        fs::rename(oldPath, newPath);
        Logger::info("Zmieniono nazwe: " + oldPath + " -> " + newPath);
        return true;

    } catch (const std::exception& e) {
        Logger::error("Blad zmiany nazwy: " + std::string(e.what()));
        return false;
    }
}

// Usuwa caly katalog wraz z zawartoscia
bool FileManager::removeDirectory(const std::string& path) {
    try {
        std::uintmax_t removed = fs::remove_all(path);
        if (removed > 0) {
            Logger::info("Usunieto katalog i " + std::to_string(removed) + " elementow: " + path);
            return true;
        }

        return false;
    } catch (const std::exception& e) {
        Logger::error("Blad usuniecia katalogu '" + path + "': " + e.what());
        return false;
    }
}

// Sprawdza czy plik lub katalog istnieje
bool FileManager::exists(const std::string& path)       { return fs::exists(path);          }
// Sprawdza czy podana sciezka to katalog
bool FileManager::isDirectory(const std::string& path)  { return fs::is_directory(path);    }


// Zapisuje tekst do pliku
bool FileManager::writeFile(const std::string& path, const std::string& data) {
    std::ofstream file(path);

    if (!file.is_open()) {
        Logger::error("Nie mozna otworzyc pliku do zapisu: " + path);
        return false;
    }

    file << data;
    if (file.good()) {
        Logger::info("Zapisano plik: " + path);
        return true;
    } else {
        Logger::error("Blad zapisu do pliku: " + path);
        return false;
    }
}

// Wczytuje caly plik jako tekst
std::string FileManager::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::warn("Nie mozna otworzyc pliku do odczytu: " + path);
        return "";
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    Logger::info("Wczytano plik (" + std::to_string(content.size()) + " bajtow): " + path);
    return content;
}

// Zapisuje dane binarne do pliku
bool FileManager::writeBinary(const std::string& path, const void* data, size_t size) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        Logger::error("Nie mozna otworzyc pliku binarnego do zapisu: " + path);
        return false;
    }

    file.write(static_cast<const char*>(data), size);
    Logger::info("Zapisano dane binarne (" + std::to_string(size) + " bajtow): " + path);
    return file.good();
}

// Wczytuje dane binarne z pliku do pamieci
bool FileManager::readBinary(const std::string& path, void* buffer, size_t size) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        Logger::warn("Nie mozna otworzyc pliku binarnego do odczytu: " + path);
        return false;
    }

    file.read(static_cast<char*>(buffer), size);
    Logger::info("Wczytano dane binarne (" + std::to_string(size) + " bajtow): " + path);
    return file.good();
}

// Zwraca rozmiar pliku w bajtach
size_t FileManager::getFileSize(const std::string& path) {
    try {
        size_t size = fs::file_size(path);
        Logger::info("Rozmiar pliku " + path + ": " + std::to_string(size) + " bajtow");
        return size;
    } catch (...) {
        Logger::warn("Nie mozna pobrac rozmiaru pliku: " + path);
        return 0;
    }
}

// Zwraca liste nazw plikow w katalogu
std::vector<std::string> FileManager::listFiles(const std::string& dir) {
    std::vector<std::string> files;

    if (!exists(dir)) {
        Logger::warn("Katalog nie istnieje: " + dir);
        return files;
    }

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }

    Logger::info("Znaleziono " + std::to_string(files.size()) + " plikow w: " + dir);
    return files;
}

// Zwraca liste nazw podkatalogow
std::vector<std::string> FileManager::listDirectories(const std::string& dir) {
    std::vector<std::string> dirs;

    if (!exists(dir)) {
        Logger::warn("Katalog nie istnieje: " + dir);
        return dirs;
    }

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_directory()) {
            dirs.push_back(entry.path().filename().string());
        }
    }

    Logger::info("Znaleziono " + std::to_string(dirs.size()) + " podkatalogow w: " + dir);
    return dirs;
}

// Usuwa pojedynczy plik
bool FileManager::deleteFile(const std::string& path) {

    if (!exists(path)) {
        Logger::warn("Plik nie istnieje, nie mozna usunac: " + path);
        return false;
    }

    if (fs::remove(path)) {
        Logger::info("Usunieto plik: " + path);
        return true;
    } else {
        Logger::error("Nie mozna usunac pliku: " + path);
        return false;
    }
}