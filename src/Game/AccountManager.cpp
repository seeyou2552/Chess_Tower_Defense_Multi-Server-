#include "Game/AccountManager.h"
#include "Game/Account.h"
#include "Crypto/SHA256.h"
#include "Core/Logger.h"
#include "Network/ErrorCode.h"

#include <nlohmann/json.hpp>
#include <random>
#include <sstream>

namespace
{
    constexpr int LoginCacheTtlSeconds = 300;

    std::string MakeLoginCacheKey(const std::string& loginId)
    {
        return "ctd:login:" + loginId;
    }
}

AccountManager::AccountManager(
    Database& database,
    RedisManager& redisManager
)
    :
    m_database(database),
    m_redisManager(redisManager)
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
    const auto cacheKey = MakeLoginCacheKey(loginId);
    std::string cachedValue;
    const auto cacheResult = m_redisManager.Get(cacheKey, cachedValue);

    if (cacheResult == RedisReadResult::Hit)
    {
        try
        {
            const auto cache = nlohmann::json::parse(cachedValue);
            auto account = std::make_shared<Account>();
            account->loginId = cache.at("loginId").get<std::string>();
            account->uuid = cache.at("uuid").get<std::string>();
            account->password = cache.at("passwordHash").get<std::string>();
            account->salt = cache.at("salt").get<std::string>();

            if (account->loginId == loginId &&
                SHA256::Hash(password + account->salt) == account->password)
            {
                error = ErrorCode::None;
                Logger::GetInstance().Info(
                    "[Login] Redis Cache Hit - loginId: " + loginId
                );
                return account;
            }

            error = ErrorCode::InvalidPassword;
            Logger::GetInstance().Info(
                "[Login] Redis Cache Hit - loginId: " + loginId
            );
            return nullptr;
        }
        catch (const nlohmann::json::exception&)
        {
            m_redisManager.Delete(cacheKey);
            Logger::GetInstance().Warning(
                "[Login] Invalid Redis cache entry - loginId: " + loginId
            );
        }
    }
    else if (cacheResult == RedisReadResult::Miss)
    {
        Logger::GetInstance().Info(
            "[Login] Redis Cache Miss - loginId: " + loginId
        );
    }
    else
    {
        Logger::GetInstance().Warning(
            "[Login] Redis unavailable, fallback to MySQL - loginId: " + loginId
        );
    }

    Logger::GetInstance().Info("[Login] MySQL Query");
    auto account = m_database.LoadAccount(loginId, password, error);
    if (account)
    {
        const nlohmann::json cache = {
            { "loginId", account->loginId },
            { "uuid", account->uuid },
            { "passwordHash", account->password },
            { "salt", account->salt }
        };

        if (m_redisManager.Set(
                cacheKey,
                cache.dump(),
                LoginCacheTtlSeconds
            ))
        {
            Logger::GetInstance().Info(
                "[Login] Redis Cache Set - loginId: " + loginId
            );
        }
    }

    return account;
}