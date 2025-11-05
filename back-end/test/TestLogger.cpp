#include "Test.h"
#include "../include/Logger.hpp"
#include <iostream>

void testLogger() {
    std::cout << "\n=== TESTUJEMY Logger ===\n";

    // // === 1. showWelcomeBanner ===
    // std::cout << "1. showWelcomeBanner() -> ";
    // showWelcomeBanner();  

    // // === 2. showHelp ===
    // std::cout << "2. showHelp() -> ";
    // showHelp();  

    // === 3. Logger ===
    std::cout << "3. Logger:\n";
    Logger::info("To jest test INFO");
    Logger::warn("To jest test WARNING");
    Logger::error("To jest test ERROR");

    std::cout << "=== KONIEC TESTOW  ===\n\n";
}