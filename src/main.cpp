#include "Game/GameServer.h"
#include "Core/Logger.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
    std::ifstream configFile("config.json");
    if (!configFile.is_open())
    {
        Logger::GetInstance().Error("Failed to open config.json");
        return -1;
    }

    std::string logFilePath;
    try
    {
        json config;
        configFile >> config;
        logFilePath = config.at("logger").at("file_path").get<std::string>();
    }
    catch (const std::exception&)
    {
        Logger::GetInstance().Error("Missing or invalid logger.file_path in config.json");
        return -1;
    }

    try
    {
        if (!Logger::GetInstance().SetFilePath(logFilePath))
        {
            Logger::GetInstance().Error("Failed to open log file: " + logFilePath);
            return -1;
        }
    }
    catch (const std::exception&)
    {
        Logger::GetInstance().Error("Failed to create log file path: " + logFilePath);
        return -1;
    }

    Logger::GetInstance().Info("Server starting...");

    GameServer server;

    Logger::GetInstance().Info("GameServer created");

    if (!server.Start())
    {
        Logger::GetInstance().Error("Server Start() FAILED");
        return -1;
    }

    Logger::GetInstance().Info("Server Start() SUCCESS");

    server.Run();

    Logger::GetInstance().Info("Server Run() returned");

    return 0;
}