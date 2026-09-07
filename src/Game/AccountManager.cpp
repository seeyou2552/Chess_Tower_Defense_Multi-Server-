#include "Game/AccountManager.h"
#include "Crypto/SHA256.h"
#include "Core/Logger.h"
#include "Network/ErrorCode.h"

#include <random>
#include <sstream>

AccountManager::AccountManager(
    Database& database
)
    :
    m_database(database)
{
}

bool AccountManager::Register(
    const std::string& loginId,
    const std::string& password,
    ErrorCode& error
)
{
    if (!m_database.ExistsAccount(loginId))
    {
        Logger::GetInstance().Info("eerr");
        return false;
    }

    auto salt =
        SHA256::GenerateSalt();

    auto hash =
        SHA256::Hash(
            password + salt
        );

    return m_database.InsertAccount(
        loginId,
        hash,
        salt,
        error
    );
}

std::shared_ptr<Account> AccountManager::Login(
    const std::string& loginId,
    const std::string& password,
    ErrorCode& error
)
{
    return m_database.LoadAccount(
        loginId,
        password,
        error
    );
}