#include "../../include/FileManager.hpp"
#include <fstream>
#include <sstream>

// Tworzy katalog
bool FileManager::createDirectory(const std::string& path) {
    try {
        if (fs::create_directories(path)) {
            #ifdef TEST
            Logger::info("Utworzono katalog: " + path);
            #endif
            return true;
        } else if (fs::exists(path)) {
            #ifdef TEST
            Logger::info("Katalog juz istnieje: " + path);
            #endif
            return true;
        }

        return false;
    } catch (const std::exception& e) {
        #ifdef TEST
        Logger::error("Blad tworzenia katalogu '" + path + "': " + e.what());
        #endif
        return false;
    }
}

// Zmienia nazwe
bool FileManager::rename(const std::string& oldPath, const std::string& newPath) {
    try {
        if (!fs::exists(oldPath)) {
            #ifdef TEST
            Logger::warn("Nie mozna zmienic nazwy: sciezka nie istnieje: " + oldPath);
            #endif
            return false;
        }

        if (fs::exists(newPath)) {
            #ifdef TEST
            Logger::warn("Nie mozna zmienic nazwy: nowa sciezka juz istnieje: " + newPath);
            #endif
            return false;
        }

        fs::rename(oldPath, newPath);
        #ifdef TEST
        Logger::info("Zmieniono nazwe: " + oldPath + " -> " + newPath);
        #endif
        return true;

    } catch (const std::exception& e) {
        #ifdef TEST
        Logger::error("Blad zmiany nazwy: " + std::string(e.what()));
        #endif
        return false;
    }
}

// Usuwa caly katalog wraz z zawartoscia
bool FileManager::removeDirectory(const std::string& path) {
    try {
        std::uintmax_t removed = fs::remove_all(path);
        if (removed > 0) {
            #ifdef TEST
            Logger::info("Usunieto katalog i " + std::to_string(removed) + " elementow: " + path);
            #endif
            return true;
        }

        return false;
    } catch (const std::exception& e) {
        #ifdef TEST
        Logger::error("Blad usuniecia katalogu '" + path + "': " + e.what());
        #endif
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
        #ifdef TEST
        Logger::error("Nie mozna otworzyc pliku do zapisu: " + path);
        #endif
        return false;
    }

    file << data;
    if (file.good()) {
        #ifdef TEST
        Logger::info("Zapisano plik: " + path);
        #endif
        return true;
    } else {
        #ifdef TEST
        Logger::error("Blad zapisu do pliku: " + path);
        #endif
        return false;
    }
}

// Wczytuje caly plik jako tekst
std::string FileManager::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        #ifdef TEST
        Logger::warn("Nie mozna otworzyc pliku do odczytu: " + path);
        #endif
        return "";
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    #ifdef TEST
    Logger::info("Wczytano plik (" + std::to_string(content.size()) + " bajtow): " + path);
    #endif
    return content;
}

// Zapisuje dane binarne do pliku
bool FileManager::writeBinary(const std::string& path, const void* data, size_t size) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        #ifdef TEST
        Logger::error("Nie mozna otworzyc pliku binarnego do zapisu: " + path);
        #endif
        return false;
    }

    file.write(static_cast<const char*>(data), size);
    #ifdef TEST
    Logger::info("Zapisano dane binarne (" + std::to_string(size) + " bajtow): " + path);
    #endif
    return file.good();
}

// Wczytuje dane binarne z pliku do pamieci
bool FileManager::readBinary(const std::string& path, void* buffer, size_t size) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        #ifdef TEST
        Logger::warn("Nie mozna otworzyc pliku binarnego do odczytu: " + path);
        #endif
        return false;
    }

    file.read(static_cast<char*>(buffer), size);
    #ifdef TEST
    Logger::info("Wczytano dane binarne (" + std::to_string(size) + " bajtow): " + path);
    #endif
    return file.good();
}

// Zwraca rozmiar pliku w bajtach
size_t FileManager::getFileSize(const std::string& path) {
    try {
        size_t size = fs::file_size(path);
        #ifdef TEST
        Logger::info("Rozmiar pliku " + path + ": " + std::to_string(size) + " bajtow");
        #endif
        return size;
    } catch (...) {
        #ifdef TEST
        Logger::warn("Nie mozna pobrac rozmiaru pliku: " + path);
        #endif
        return 0;
    }
}

// Zwraca liste nazw plikow w katalogu
std::vector<std::string> FileManager::listFiles(const std::string& dir) {
    std::vector<std::string> files;

    if (!exists(dir)) {
        #ifdef TEST
        Logger::warn("Katalog nie istnieje: " + dir);
        #endif
        return files;
    }

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }

    #ifdef TEST
    Logger::info("Znaleziono " + std::to_string(files.size()) + " plikow w: " + dir);
    #endif
    return files;
}

// Zwraca liste nazw podkatalogow
std::vector<std::string> FileManager::listDirectories(const std::string& dir) {
    std::vector<std::string> dirs;

    if (!exists(dir)) {
        #ifdef TEST
        Logger::warn("Katalog nie istnieje: " + dir);
        #endif
        return dirs;
    }

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_directory()) {
            dirs.push_back(entry.path().filename().string());
        }
    }

    #ifdef TEST
    Logger::info("Znaleziono " + std::to_string(dirs.size()) + " podkatalogow w: " + dir);
    #endif
    return dirs;
}

// Usuwa pojedynczy plik
bool FileManager::deleteFile(const std::string& path) {

    if (!exists(path)) {
        #ifdef TEST
        Logger::warn("Plik nie istnieje, nie mozna usunac: " + path);
        #endif
        return false;
    }

    if (fs::remove(path)) {
        #ifdef TEST
        Logger::info("Usunieto plik: " + path);
        #endif
        return true;
    } else {
        #ifdef TEST
        Logger::error("Nie mozna usunac pliku: " + path);
        #endif
        return false;
    }
}
