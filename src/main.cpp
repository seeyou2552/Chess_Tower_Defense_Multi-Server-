#include "Game/GameServer.h"
#include "Core/Logger.h"

using json = nlohmann::json;

int main()
{
    GameServer server;

    if (!server.Start())
        return -1;

    server.Run();

    return 0;
}