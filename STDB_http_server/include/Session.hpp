#pragma once

#define DEFAULT_ADMIN_PASSWORD "admin"
#define TOKEN_BYTE_LENGTH 16

#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>

class Session {
    private:
        static std::string hashPassword(const std::string& password);
        static bool verifyPassword(const std::string& password, const std::string& hash);
        static std::string token_b64url();
        std::vector<std::string> activeSessions;
        mutable std::shared_mutex m;
    public:
        Session();
        std::string createSession(const std::string& password);
        bool isValidSession(const std::string& token) const;
        void destroySession(const std::string& token);
};