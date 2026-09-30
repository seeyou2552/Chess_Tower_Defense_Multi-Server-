#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>

#include <sw/redis++/redis++.h>

struct RedisConfig
{
    std::string host = "127.0.0.1";
    uint16_t port = 6379;
    std::string password;
    std::chrono::milliseconds connectTimeout{ 250 };
    std::chrono::milliseconds socketTimeout{ 250 };
};

enum class RedisReadResult
{
    Hit,
    Miss,
    Unavailable
};

class RedisManager
{
public:
    RedisManager() = default;
    ~RedisManager();

    bool Connect(const RedisConfig& config);
    bool IsConnected() const;

    bool Set(
        const std::string& key,
        const std::string& value,
        int expireSeconds = 0
    );

    RedisReadResult Get(
        const std::string& key,
        std::string& value
    );

    bool Delete(const std::string& key);
    bool Exists(const std::string& key);

private:
    void MarkUnavailable(const std::exception& error);

    std::unique_ptr<sw::redis::Redis> m_redis;
    std::atomic<bool> m_connected = false;
};