#include "Core/Logger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

bool Logger::SetFilePath(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::filesystem::path path(filePath);
    if (path.has_parent_path())
    {
        std::filesystem::create_directories(path.parent_path());
    }

    m_file.close();
    m_file.open(path, std::ios::app);
    return m_file.is_open();
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

    std::ostringstream line;
    line << std::put_time(&tm, "%Y/%m/%d/%H:%M:%S") << " - ";

    switch (level)
    {
    case LogLevel::Info:
        line << "[INFO] ";
        break;

    case LogLevel::Warning:
        line << "[WARN] ";
        break;

    case LogLevel::Error:
        line << "[ERROR] ";
        break;
    }

    line << message;
    std::cout << line.str() << std::endl;
    if (m_file.is_open())
    {
        m_file << line.str() << std::endl;
    }
}