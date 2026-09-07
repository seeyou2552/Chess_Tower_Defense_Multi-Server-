#pragma once

#include <memory>
#include <string>

#include <mysqlx/xdevapi.h>

#include "Game/Account.h"
#include "Network/ErrorCode.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct DBConfig {
    std::string host;
    int port;
    std::string user;
    std::string password;
    std::string database;
};

class Database
{
public:
    Database();
    ~Database();

    bool Connect(const DBConfig& config);

    bool ExistsAccount(const std::string& loginId);

    std::shared_ptr<Account> LoadAccount(
        const std::string& loginId,
        const std::string& password,
        ErrorCode& error
    );

    bool InsertAccount(
        const std::string& loginId,
        const std::string& password,
        const std::string& salt,
        ErrorCode& error
    );

private:
    std::unique_ptr<mysqlx::Session> m_session;
};