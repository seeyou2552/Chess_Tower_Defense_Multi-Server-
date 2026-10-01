#include "Core/Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

void Logger::Info(const std::string& message)
{
    Write(LogLevel::Info, message);
}

void Logger::Warning(const std::string& message)
{
    Write(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message)
{
    Write(LogLevel::Error, message);
}

void Logger::Write(LogLevel level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::cout << "["
              << std::put_time(&tm, "%H:%M:%S")
              << "] ";

    switch (level)
    {
    case LogLevel::Info:
        std::cout << "[INFO] ";
        break;

    case LogLevel::Warning:
        std::cout << "[WARN] ";
        break;

    case LogLevel::Error:
        std::cout << "[ERROR] ";
        break;
    }

    std::cout << message << std::endl;
}