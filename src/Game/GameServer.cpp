#include "Game/GameServer.h"
#include "Core/Logger.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Network/Database.h"
#include "Game/Handler/LoginHandler.h"
#include "Game/Handler/PingHandler.h"
#include "Game/Handler/JoinRoomHandler.h"
#include "Game/Handler/RegisterHandler.h"
#include "Game/Handler/LoadSceneHandler.h"
#include "Game/Handler/SpawnMinionHandler.h"
#include "Game/Handler/RelocateMinionHandler.h"
#include "Game/Handler/WaveHandler.h"
#include "Game/Handler/SellMinionHandler.h"
#include "Game/Handler/UpgradeMinionHandler.h"

#include "Game/Minion/MinionDataManager.h"
#include "Game/Enemy/EnemyDataManager.h"
#include "Game/Skill/SkillDataManager.h"
#include "Game/Minion/StatUpgradeManager.h"
#include "Game/Skill/SkillManager.h"

GameServer::GameServer()
    :
    m_config(),
    m_server(
        m_ioContext,
        *this,
        m_dispatcher,
        m_config
    ),
    m_stageManager(),
    m_tickManager(),
    m_gameManager(
        m_tickManager
    ),
    m_accountManager(
        m_database
    ),
    m_objectPoolManager(),
    m_spawnManager(
        m_objectPoolManager
    ),
    m_roomManager(
        m_tickManager,
        m_stageManager,
        m_spawnManager
    )
{
    auto loginHandler =
        std::make_shared<LoginHandler>();

    m_dispatcher.Register(
        PacketType::LoginRequest,
        [loginHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
        )
        {
            loginHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto pingHandler =
        std::make_shared<PingHandler>();

    m_dispatcher.Register(
        PacketType::PingRequest,

        [pingHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
        )
        {
            pingHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto joinRoomHandler =
    std::make_shared<JoinRoomHandler>();


    m_dispatcher.Register(
        PacketType::JoinRoomRequest,

        [joinRoomHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
        )
        {
            joinRoomHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto registerHandler =
        std::make_shared<RegisterHandler>();


    m_dispatcher.Register(
        PacketType::RegisterRequest,

        [registerHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
        )
        {
            registerHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto loadSceneHandler =
        std::make_shared<LoadSceneHandler>();

    m_dispatcher.Register(
        PacketType::LoadSceneResponse,

        [loadSceneHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
            )
        {
            loadSceneHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto spawnMinionHandler =
        std::make_shared<SpawnMinionHandler>();

    m_dispatcher.Register(
        PacketType::SpawnMinionRequest,

        [spawnMinionHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
            )
        {
            spawnMinionHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto relocateMinionHandler =
        std::make_shared<RelocateMinionHandler>();

    m_dispatcher.Register(
        PacketType::RelocateMinionRequest,

        [relocateMinionHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
            )
        {
            relocateMinionHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto startWaveHandler =
        std::make_shared<WaveHandler>();

    m_dispatcher.Register(
        PacketType::StartWaveRequest,

        [startWaveHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
            )
        {
            startWaveHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto sellMinionHandler =
        std::make_shared<SellMinionHandler>();

    m_dispatcher.Register(
        PacketType::SellMinionRequest,

        [sellMinionHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
            )
        {
            sellMinionHandler->Handle(
                session,
                packet,
                size
            );
        }
    );

    auto upgradeMinionHandler =
        std::make_shared<UpgradeMinionHandler>();

    m_dispatcher.Register(
        PacketType::UpgradeMinionRequest,

        [upgradeMinionHandler]
        (
            std::shared_ptr<Session> session,
            const char* packet,
            uint16_t size
            )
        {
            upgradeMinionHandler->Handle(
                session,
                packet,
                size
            );
        }
    );
}

bool GameServer::Start()
{
    Logger::GetInstance().Info("GameServer Start");

    m_running = true;

    DBConfig dbConfig;

    try {
        // 1. config.json 파일 열기
        std::ifstream configFile("config.json");
        if (!configFile.is_open()) {
            Logger::GetInstance().Info("config Err");
            return -1;
        }

        // 2. JSON 파싱
        json j;
        configFile >> j;

        // 3. 구조체에 값 매핑 (json 객체에서 데이터 추출)
        dbConfig.host = j["database"]["host"];
        dbConfig.port = j["database"]["port"];
        dbConfig.user = j["database"]["user"];
        dbConfig.password = j["database"]["password"];
        dbConfig.database = j["database"]["dbname"];

    }
    catch (const std::exception& e) {
        Logger::GetInstance().Info("Error parsing config.json");

        return -1;
    }

    // Database 객체 생성 및 연결 시도
    if (!m_database.Connect(dbConfig)) {
        Logger::GetInstance().Info("Server failed to start due to database connection error.");
        return -1;
    }

	// Stage 데이터 로드
    if (!m_stageManager.Load("StageData.json"))
    {
        Logger::GetInstance().Error(
            "Stage Load Failed"
        );

        return -1;
    }

    if (!m_stageManager.LoadAllStageTiles())
    {
		Logger::GetInstance().Error(
			"Stage Tile Load Failed"
		);
		return -1;
    }

    // Minion 데이터 로드
    if (!MinionDataManager::GetInstance().Load("MinionData.json"))
    {
        Logger::GetInstance().Error(
            "Minion Data Load Failed"
        );

        return -1;
    }

    // Enemy 데이터 로드
    if (!EnemyDataManager::GetInstance().Load("EnemiesData.json"))
    {
        Logger::GetInstance().Error(
            "Enemy Data Load Failed"
        );

        return -1;
    }

    // Skill 데이터 로드
    if (!SkillDataManager::GetInstance().Load("SkillData.json"))
    {
        Logger::GetInstance().Error(
            "Skill Data Load Failed"
        );

        return -1;
    }

    // UpgradeManager
    StatUpgradeManager::GetInstance().Init();

    // SkillManager
    SkillManager::GetInstance().Init();

    return m_server.Start(7777);
}

void GameServer::Stop()
{
    m_running = false;
}

void GameServer::Run()
{
    using clock =
        std::chrono::steady_clock;


    auto last =
        clock::now();


    while(m_running)
    {
        m_ioContext.poll();


        auto now =
            clock::now();


        float deltaTime =
            std::chrono::duration<float>(
                now - last
            ).count();


        last = now;


        m_tickManager.Update(
            deltaTime
        );


        std::this_thread::sleep_for(
            std::chrono::milliseconds(1)
        );
    }
}