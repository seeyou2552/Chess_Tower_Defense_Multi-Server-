#include "Game/GameServer.h"
#include "Core/Logger.h"

#include <iostream>

using json = nlohmann::json;

int main()
{
    std::cout << "[MAIN] Server starting..." << std::endl;

    GameServer server;

    std::cout << "[MAIN] GameServer created" << std::endl;

    if (!server.Start())
    {
        std::cout << "[MAIN] Server Start() FAILED" << std::endl;
        return -1;
    }

    std::cout << "[MAIN] Server Start() SUCCESS" << std::endl;

    server.Run();

    std::cout << "[MAIN] Server Run() returned" << std::endl;

    return 0;
}