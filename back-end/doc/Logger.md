# Logger 

`Logger` to klasa do wyświetlania komunikatów

---

## Główne funkcje

| Funkcja | Co robi? | Przykład użycia |
|--------|----------|-----------------|
| `Logger::info("Start")`   | Zielony komunikat `[INFO]`        | ```Logger::info("Baza utworzona!");```    |
| `Logger::warn("Uwaga")`   | Żółty komunikat `[WARNING]`       | ```Logger::warn("Plik nie istnieje");```  |
| `Logger::error("Blad")`   | Czerwony komunikat `[ERROR]`      | ```Logger::error("Brak pamieci");```      |
| `showWelcomeBanner()`     | Pokazuje  banner powitalny   | ```showWelcomeBanner();```                |
| `showHelp()`              | Pokazuje listę komend             | ```showHelp();```                         |

---

## Kolory – gdzie działają?

| Terminal | Kolory | Wygląd |
|--------|--------|-------|
| **CMD (Windows)** | Tak | Zielony, żółty, czerwony |
| **PowerShell** | Tak | Kolory ANSI |
| **MSYS2 / Git Bash** | Tak | Kolory ANSI |
| **Linux Terminal** | Tak | Kolory ANSI |

---

## Przykład: Logowanie z kolorami

```cpp
#include "../../include/Logger.h"

int main() {
    showWelcomeBanner();

    Logger::info("Program uruchomiony");
    Logger::warn("Plik tymczasowy");
    Logger::error("Nie mozna zapisac");

    showHelp();
    return 0;
}