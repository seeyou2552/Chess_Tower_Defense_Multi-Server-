#pragma once

#include <memory>
#include <string>

#include "Network/Database.h"
#include "Network/ErrorCode.h"

class Account;

class AccountManager
{
public:
    explicit AccountManager(
        Database& database
    );

    bool Register(
        const std::string& loginId,
        const std::string& password,
        ErrorCode& error
    );

    std::shared_ptr<Account> Login(
        const std::string& loginId,
        const std::string& password,
        ErrorCode& error
    );

private:
    Database& m_database;
};