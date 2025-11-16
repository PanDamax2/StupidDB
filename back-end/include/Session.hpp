#pragma once

#define DEFAULT_ADMIN_PASSWORD "admin"
#define TOKEN_BYTE_LENGTH 16

#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <map>
#include "QueryExecutor.hpp"

using tokenParserType = std::map<std::string, std::shared_ptr<QueryExecutor>>;

class Session {
private:
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
    static std::string token_b64url();
    mutable std::shared_mutex m;
    tokenParserType parserSessions;
public:
    Session();
    std::string createSession(const std::string& password);
    bool isValidSession(const std::string& token) const;
    void destroySession(const std::string& token);
    bool changePassword(const std::string& password);
    std::shared_ptr<QueryExecutor> getQueryExecutor(const std::string& token);
};