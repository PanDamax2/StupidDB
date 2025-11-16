#include "../../include/Session.hpp"
#include "../../include/Logger.hpp"
#include "../../include/FileManager.hpp"

#include <sodium.h>
#include <vector>
#include <string>
#include <algorithm>

std::string Session::hashPassword(const std::string& password) {
    if(sodium_init() < 0) {
        Logger::error("Nie mozna zainicjowac libsodium do hashowania hasla.");
        return "";
    }

    char hash[crypto_pwhash_STRBYTES];
    if(crypto_pwhash_str(hash, password.c_str(), password.length(),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        Logger::error("Blad podczas hashowania hasla.");
        return "";
    }

    return std::string(hash);
}

bool Session::verifyPassword(const std::string& password, const std::string& hash) {
    if(sodium_init() < 0) {
        Logger::error("Nie mozna zainicjowac libsodium do weryfikacji hasla.");
        return false;
    }

    if(crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0) {
        return true; // Haslo poprawne
    } else {
        return false; // Haslo niepoprawne
    }
}

std::string Session::token_b64url() {
    if(sodium_init() < 0) {
        Logger::error("Nie mozna zainicjowac libsodium do weryfikacji hasla.");
        return "";
    }

    std::vector<unsigned char> buf(TOKEN_BYTE_LENGTH);
    randombytes_buf(buf.data(), buf.size());

    // Maksymalny rozmiar base64 (z paddingiem) to 4/3 * TOKEN_BYT_LENGTH + 4
    // Użyjemy funkcji sodium do bezpiecznego kodowania URL-safe:
    // Wariant bez paddingu → czysty do URL.
    std::size_t out_len = sodium_base64_ENCODED_LEN(
        (unsigned long long)buf.size(),
        sodium_base64_VARIANT_URLSAFE_NO_PADDING
    );
    std::string out(out_len, '\0');

    sodium_bin2base64(out.data(), out.size(), buf.data(), buf.size(), sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    // sodium_* zwraca C-string zakończony '\0' → usuń trailing '\0' jeśli jest
    if (!out.empty() && out.back() == '\0') out.pop_back();
    return out;
}

Session::Session() {
    if(!FileManager::exists("password.txt")) {
        if(FileManager::writeFile("password.txt", hashPassword(DEFAULT_ADMIN_PASSWORD)))
            Logger::info("Utworzono domyslne haslo administratora: " + std::string(DEFAULT_ADMIN_PASSWORD) + ". Zmien je natychmiast po zalogowaniu.");
        else {
            Logger::error("Nie mozna utworzyc domyslnego hasla administratora.");
            return;
        }  
    }
}

std::string Session::createSession(const std::string& password) {
    std::unique_lock lock(m);
    std::string storedHash = FileManager::readFile("password.txt");
    if(!verifyPassword(password, storedHash)) {
        Logger::warn("Nieudana proba utworzenia sesji: niepoprawne haslo.");
        return "";
    }

    std::string token = token_b64url();
    parserSessions[token] = std::make_shared<QueryExecutor>();
    return token;
}

bool Session::isValidSession(const std::string& token) const {
    std::shared_lock lock(m);
    return parserSessions.find(token) != parserSessions.end();
}

void Session::destroySession(const std::string& token) {
    std::unique_lock lock(m);
    parserSessions.erase(token);
}

bool Session::changePassword(const std::string& password) {
    std::unique_lock lock(m);
    return FileManager::writeFile("password.txt", hashPassword(password));
}

std::shared_ptr<QueryExecutor> Session::getQueryExecutor(const std::string& token) {
    std::shared_lock lock(m);
    if(parserSessions.find(token) == parserSessions.end()) {
        return nullptr;
    }
    return parserSessions[token];
}