#include "Network/RedisManager.h"

#include "Core/Logger.h"

#include <sw/redis++/redis++.h>

RedisManager::~RedisManager() = default;

bool RedisManager::Connect(const RedisConfig& config)
{
    m_connected = false;

    try
    {
        sw::redis::ConnectionOptions options;
        options.host = config.host;
        options.port = config.port;
        options.connect_timeout = config.connectTimeout;
        options.socket_timeout = config.socketTimeout;

        if (!config.password.empty())
        {
            options.password = config.password;
        }

        m_redis = std::make_unique<sw::redis::Redis>(options);
        m_redis->ping();
        m_connected = true;
        Logger::GetInstance().Info("[Redis] Connected");
        return true;
    }
    catch (const std::exception& error)
    {
        MarkUnavailable(error);
        return false;
    }
}

bool RedisManager::IsConnected() const
{
    return m_connected;
}

bool RedisManager::Set(
    const std::string& key,
    const std::string& value,
    int expireSeconds
)
{
    if (!m_redis)
    {
        m_connected = false;
        return false;
    }

    try
    {
        if (expireSeconds > 0)
        {
            m_redis->set(
                key,
                value,
                std::chrono::milliseconds(expireSeconds) * 1000
            );
        }
        else
        {
            m_redis->set(key, value);
        }

        m_connected = true;
        return true;
    }
    catch (const std::exception& error)
    {
        MarkUnavailable(error);
        return false;
    }
}

RedisReadResult RedisManager::Get(
    const std::string& key,
    std::string& value
)
{
    if (!m_redis)
    {
        m_connected = false;
        return RedisReadResult::Unavailable;
    }

    try
    {
        auto cachedValue = m_redis->get(key);
        m_connected = true;

        if (!cachedValue)
        {
            return RedisReadResult::Miss;
        }

        value = *cachedValue;
        return RedisReadResult::Hit;
    }
    catch (const std::exception& error)
    {
        MarkUnavailable(error);
        return RedisReadResult::Unavailable;
    }
}

bool RedisManager::Delete(const std::string& key)
{
    if (!m_redis)
    {
        m_connected = false;
        return false;
    }

    try
    {
        m_redis->del(key);
        m_connected = true;
        return true;
    }
    catch (const std::exception& error)
    {
        MarkUnavailable(error);
        return false;
    }
}

bool RedisManager::Exists(const std::string& key)
{
    if (!m_redis)
    {
        m_connected = false;
        return false;
    }

    try
    {
        const bool exists = m_redis->exists(key) != 0;
        m_connected = true;
        return exists;
    }
    catch (const std::exception& error)
    {
        MarkUnavailable(error);
        return false;
    }
}

void RedisManager::MarkUnavailable(const std::exception& error)
{
    m_connected = false;
    Logger::GetInstance().Warning(
        "[Redis] Connection Failed: " + std::string(error.what())
    );
}