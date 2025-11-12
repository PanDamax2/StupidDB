#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "./Logger.hpp"

namespace fs = std::filesystem;

class FileManager {
public:
    // === OPERACJE NA KATALOGACH ===
    static bool createDirectory(const std::string& path);
    static bool removeDirectory(const std::string& path);
    static bool rename(const std::string& oldPath, const std::string& newPath);
    
    // === SPRAWDZANIE ISTNIENIA ===
    static bool exists(const std::string& path);
    static bool isDirectory(const std::string& path);
    
    // === OPERACJE NA PLIKACH TEKSTOWYCH ===
    static bool writeFile(const std::string& path, const std::string& data);
    static std::string readFile(const std::string& path);
    
    // === OPERACJE NA PLIKACH BINARNYCH ===
    static bool writeBinary(const std::string& path, const void* data, size_t size);
    static bool readBinary(const std::string& path, void* data, size_t size);
    static size_t getFileSize(const std::string& path);
    
    // === LISTOWANIE ===
    static std::vector<std::string> listFiles(const std::string& path);
    static std::vector<std::string> listDirectories(const std::string& path);
    
    // === USUWANIE ===
    static bool deleteFile(const std::string& path);
};