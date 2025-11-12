#pragma once
#include <string>

enum class LogLevel { Info, Warning, Error };

void showWelcomeBanner();
void showHelp();

class Logger {
public:
    // Logowanie wiadomość
    static void log(LogLevel level, const std::string& message);

    // Skróty dla logow
    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);
};