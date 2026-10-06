#pragma once

#include <fstream>
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

    bool SetFilePath(const std::string& filePath);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);

private:
    Logger() = default;

    void Write(LogLevel level, const std::string& message);

    std::mutex m_mutex;
    std::ofstream m_file;
};