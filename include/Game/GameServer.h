#pragma once

#include <asio.hpp>

#include "Network/TcpServer.h"
#include "Network/Packet/PacketDispatcher.h"
#include "Game/PlayerManager.h"
#include "Config/ServerConfig.h"
#include "Game/AccountManager.h"
#include "Game/RoomManager.h"
#include "Game/TickManager.h"
#include "Game/GameManager.h"
#include "Network/Database.h"
#include "Network/RedisManager.h"
#include "Game/Stage/StageManager.h"
#include "Game/SpawnManager.h"
#include "Game/Pool/ObjectPoolManager.h"

class GameServer
{
public:

    GameServer();
    bool Start();
    void Run();
    void Stop();

    PacketDispatcher& GetDispatcher()
    {
        return m_dispatcher;
    }

    PlayerManager& GetPlayerManager()
    {
        return m_playerManager;
    }

    AccountManager& GetAccountManager()
    {
        return m_accountManager;
    }

    RedisManager& GetRedisManager()
    {
        return m_redisManager;
    }

    RoomManager& GetRoomManager()
    {
        return m_roomManager;
    }

    TickManager& GetTickManager()
    {
        return m_tickManager;
    }

    GameManager& GetGameManager()
    {
        return m_gameManager;
    }

    StageManager& GetStageManager()
    {
        return m_stageManager;
    }

    SpawnManager& GetSpawnManager()
    {
		return m_spawnManager;
    }

	ObjectPoolManager& GetObjectPoolManager()
	{
		return m_objectPoolManager;
	}

private:

    bool m_running;

    ServerConfig m_config;
    asio::io_context m_ioContext;
    PacketDispatcher m_dispatcher;

    TickManager m_tickManager;
    StageManager m_stageManager;

    ObjectPoolManager m_objectPoolManager;
    SpawnManager m_spawnManager;

    GameManager m_gameManager;
    RoomManager m_roomManager;

    PlayerManager m_playerManager;

    Database m_database;
    RedisManager m_redisManager;
    AccountManager m_accountManager;

    TcpServer m_server;
};