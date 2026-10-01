#pragma once

#include <mutex>
#include <string>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

class Logger
{
public:
    static Logger& GetInstance();

    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);

private:
    Logger() = default;

    void Write(LogLevel level, const std::string& message);

private:
    std::mutex m_mutex;
};