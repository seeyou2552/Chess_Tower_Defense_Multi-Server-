#include "Game/Room.h"
#include "Game/Player.h"
#include "Network/Session.h"
#include "Game/GameInstance.h"
#include "Game/GameState.h"
#include "Game/Stage/StageData.h"
#include "Game/TickManager.h"
#include "Game/SpawnManager.h"
#include "Game/WaveManager.h"
#include "Game/Minion/Minion.h"
#include "Game/Minion/MinionStat.h"
#include "Game/Minion/MinionDataManager.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Math/Vector3.h"
#include "Game/Math/Timestamp.h"
#include "Game/Skill/Projectile.h"

#include "Core/Logger.h"

#include "Network/ErrorCode.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Packet/PacketType.h"
#include "Network/Packet/StagePacket.h"
#include "Network/Packet/WavePacket.h"
#include "Network/Packet/MinionPacket.h"
#include "Network/Packet/EnemyPacket.h"
#include "Network/Packet/PlayerPacket.h"



Room::Room(
    uint32_t id,
    TickManager& tickManager,
    SpawnManager& spawnManager,
    std::shared_ptr<StageData> stageData,
    std::shared_ptr<StageTileData> stageTileData
)
    :
    m_id(id),
    m_tickManager(tickManager),
    m_spawnManager(spawnManager),
    m_stageData(stageData),
    m_stageTileData(stageTileData),
    m_enemyManager(spawnManager),
    m_minionManager(
        spawnManager,
        m_enemyManager
    ),
    m_waveManager(
        *this,
        m_enemyManager,
        stageData
    ),
    m_projectileManager(
        spawnManager,
        m_enemyManager
    )
{
    m_hp = 100;
    m_gameState = GameState::Intermission;
}

void Room::Update(float deltaTime)
{
    ProcessCommands();

    std::unique_lock<std::recursive_mutex> lock(m_roomMutex);

    if (m_gameState == GameState::Intermission)
    {

    }

    else if (m_gameState == GameState::Wave)
    {
        m_waveManager.Update(deltaTime);
        m_enemyManager.Update(deltaTime);
        m_minionManager.Update(deltaTime);

        m_syncTimer += deltaTime;

        if (m_syncTimer >= 0.05f)
        {
            m_syncTimer -= 0.05f;

            lock.unlock();
            BroadcastAttack();
            lock.lock();
        }
    }

    m_projectileManager.Update(deltaTime);
}

void Room::EnqueueCommand(
    std::function<void()> command
)
{
    {
        std::lock_guard<std::mutex> lock(m_commandMutex);

        m_commandQueue.push(
            std::move(command)
        );
    }
}

void Room::ProcessCommands()
{
    std::queue<std::function<void()>> commands;

    {
        std::lock_guard<std::mutex> lock(m_commandMutex);

        std::swap(
            commands,
            m_commandQueue
        );
    }

    while (!commands.empty())
    {
        auto command = std::move(commands.front());
        commands.pop();

        try
        {
            command();
        }
        catch (const std::exception& e)
        {
            Logger::GetInstance().Info(
                std::string("Room Command Exception: ") + e.what()
            );
        }
        catch (...)
        {
            Logger::GetInstance().Info(
                "Room Command Unknown Exception"
            );
        }
    }
}

void Room::Enter( std::shared_ptr<Player> player )
{
    std::unique_lock<std::recursive_mutex> lock(m_roomMutex);

    if (!player)
    {
        Logger::GetInstance().Info("Error: Player is nullptr");
        return;
    }

    auto playerId = player->GetId();
    
	// Player 객체를 안전하게 복사하여 로컬 벡터에 저장
    std::vector<std::shared_ptr<Player>> players;

    m_players.emplace(
        playerId,
        player
    );

    for (const auto& [id, currentPlayer] : m_players)
    {
        if (currentPlayer)
        {
            players.push_back(currentPlayer);
        }
    }

    Logger::GetInstance().Info("Player entered room: " + std::to_string(playerId));

    player->SetRoom(
        shared_from_this()
    );

	// 플레이어가 로딩을 완료하지 않은 상태로 초기화
    m_playerReady[playerId] = false;

    Logger::GetInstance().Info(
        "Room Player Count : " +
        std::to_string(players.size())
    );

    // 플레이어가 2명이 되면 게임 시작
    const bool shouldStartStage = players.size() == 2;

    if (shouldStartStage)
    {
        Logger::GetInstance().Info(
            "Start?"
        );
        if (!m_game)
        {
            m_game = std::make_shared<GameInstance>(
                shared_from_this()
            );
        }

        lock.unlock();
        StageStart();
    }
}

void Room::Leave( std::shared_ptr<Player> player )
{
    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    m_players.erase(
        player->GetId()
    );

    player->SetRoom(
        nullptr
    );

    Logger::GetInstance().Info(
        "Room Player Count : " +
        std::to_string(
            m_players.size()
        )
    );
}

void Room::Broadcast( const std::vector<char>& packet )
{
    std::vector<std::shared_ptr<Session>> sessions;

    {
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

        for (const auto& [id, player] : m_players)
        {
            if (!player)
            {
                Logger::GetInstance().Error("Player nullptr");
                continue;
            }

            auto session = player->GetSession();
            if (session)
            {
                sessions.push_back(std::move(session));
            }
            else
            {
                Logger::GetInstance().Error("Session nullptr");
            }
        }
    }

    for (const auto& session : sessions)
    {
        session->Send(packet);
    }
}

std::vector<std::shared_ptr<Player>> Room::GetPlayersSnapshot() const
{
    std::vector<std::shared_ptr<Player>> players;

    {
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);
        players.reserve(m_players.size());

        for (const auto& [id, player] : m_players)
        {
            if (player)
            {
                players.push_back(player);
            }
        }
    }

    return players;
}

void Room::BroadcastGoldUpdate()
{
    std::vector<std::shared_ptr<Player>> players;

    {
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);
        for (const auto& [id, player] : m_players)
        {
            players.push_back(player);
        }
    }

    for (auto& player : players)
    {
        UpdateGold response;

        response.currentGold = player->GetGold();

        PacketWriter writer(
            PacketType::UpdateGold
        );

        writer.Write(response);
        writer.Finish();

        player->GetSession()->Send(
            writer.GetBuffer()
        );
    }
}

uint64_t Room::GetTimestamp()
{
    return m_tickManager.GetTimestamp();
}

void Room::StageStart()
{
    std::vector<std::shared_ptr<Player>> players;

    {
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);
        for (const auto& [id, player] : m_players)
        {
            if (player)
            {
                players.push_back(player);
            }
        }
    }

    for (auto& player : players)
    {
        auto session = player->GetSession();

        if (session)
        {
            PacketWriter writer(PacketType::StageStartResponse);

            StageStartResponse response{};

            response.gold = player->GetGold();
            response.orderId = player->GetId();
            response.errCode = ErrorCode::None;

            writer.Write(response);
            writer.Finish();

            session->Send(
                writer.GetBuffer()
            );
        }
        else
        {
            Logger::GetInstance().Error("Session nullptr");
        }
    }

}

bool Room::PlayerReadyCheck(std::shared_ptr<Player> player)
{
    if (!player)
    {
        return false;
    }

    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    m_playerReady[player->GetId()] = true;

    for (const auto& [id, ready] : m_playerReady)
    {
        if (!ready)
        {
            return false;
        }
    }

    // 다음 사용을 위해 모든 레디 상태를 다시 false로 초기화
    for (auto& [id, ready] : m_playerReady)
    {
        ready = false;
    }

    return true;
}

ErrorCode Room::SpawnMinion(
    uint32_t minionId,
    std::shared_ptr<Player>& player,
    int x,
    int y
)
{
    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    Vector3Int pos{ x, y, 0 };

    // 설치가 가능한 State인지
    if (m_gameState != GameState::Intermission)
        return ErrorCode::InvalidState;

    // 설치 가능한 타일인지
    if (!m_stageTileData->buildableTiles.contains(pos))
        return ErrorCode::InvalidSpawnPosition;

    // 이미 다른 미니언이 있는지
    if (m_occupiedTiles.contains(pos))
        return ErrorCode::SpawnedMinionPosition;
    
    // 잘못된 Minion Id 인지
    const MinionData* minionData = MinionDataManager::GetInstance().GetData(minionId);
    if (!minionData)
        return ErrorCode::InvalidMinionId;

    // 골드가 부족한지
    if (!player->TryUseGold(minionData->cost))
        return ErrorCode::GoldNotEnough;

    std::shared_ptr<Minion> minion = 
        m_minionManager.SpawnMinion(
            minionId,
            player->GetId(),
            x,
            y
        );

    if (!minion)
        return ErrorCode::InvalidMinionId;

    // Minion의 OnAttackCallback 설정 - C# DefaultAttack 로직 반영
    minion->SetOnDefaultAttackCallback(
        [this](
            uint32_t minioninstanceId,
            std::vector<std::shared_ptr<Enemy>> targets
        )
        {
            DefaultAttackHandle(
                minioninstanceId,
                targets
            );
        }
    );

    minion->SetOnEnemyTargetSkillCallback(
        [this](
            uint32_t minioninstanceId,
            std::vector<std::shared_ptr<Enemy>> targets
        )
        {
            EnemyTargetSkillHandle(minioninstanceId, targets);
        }
    );

    minion->SetOnMinionTargetSkillCallback(
        [this](
            uint32_t minioninstanceId,
            std::vector<std::shared_ptr<Minion>> targets
            )
        {
            MinionTargetSkillHandle(minioninstanceId, targets);
        }
    );

    m_occupiedTiles.emplace(
        pos,
        minion->GetInstanceId());

	// 미니언 스폰 알림 패킷 생성 및 브로드캐스트
    SpawnMinionNotify notify;

    notify.instanceId = minion->GetInstanceId();

    notify.minionId = minion->GetDataId();

    notify.orderId = player->GetId();

    notify.x = x;
    notify.y = y;

    PacketWriter writer(
        PacketType::SpawnMinionNotify
    );

    writer.Write(notify);
    writer.Finish();

    Broadcast(
        writer.GetBuffer()
    );

    return ErrorCode::None;
}

void Room::ReturnMinion(uint32_t instanceId)
{
    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    auto minion = m_minionManager.FindMinion(instanceId);
    if (!minion)
        return;

    Vector3 currentPos = minion->GetPosition();

    Vector3Int pos = Vector3Int(
        static_cast<int>(currentPos.x * 2.0f),
        static_cast<int>(currentPos.y * 2.0f)
    );

    m_occupiedTiles.erase(pos);

    m_minionManager.RemoveMinion(instanceId);

    // 미니언 Return 알림 패킷 생성 및 브로드캐스트
    ReturnMinionNotify notify;
    notify.instanceId = instanceId;

    PacketWriter writer(
        PacketType::ReturnMinionNotify
    );

    writer.Write(notify);
    writer.Finish();

    Broadcast(
        writer.GetBuffer()
    );
}

ErrorCode Room::RelocateMinion(
    uint32_t instanceId,
    uint32_t orderId,
    int x,
    int y
)
{
    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    Vector3Int newPos{ x, y, 0 };

    // 설치가 가능한 State인지
    if (m_gameState != GameState::Intermission)
        return ErrorCode::InvalidState;

    // 설치 가능한 타일인지
    if (!m_stageTileData->buildableTiles.contains(newPos))
        return ErrorCode::InvalidSpawnPosition;

    // 이미 다른 미니언이 있는지
    if (m_occupiedTiles.contains(newPos))
        return ErrorCode::SpawnedMinionPosition;

    // 관리 중인 룸에서 해당 미니언 찾기
    std::shared_ptr<Minion> minion = m_minionManager.FindMinion(instanceId);
    if (!minion)
        return ErrorCode::InvalidMinionId;

    // 해당 미니언의 소유주 인지
    if (minion->GetOrderId() != orderId)
        return ErrorCode::InvalidOrderId;

    Logger::GetInstance().Info(std::to_string(minion->GetInstanceId()));

    // 기존에 차지하고 있던 타일 정보를 점유 해제
    Vector3 floatPos = minion->GetPosition();

    Vector3Int oldPos(
        static_cast<int>((floatPos.x * 2)),
        static_cast<int>((floatPos.y * 2)),
        static_cast<int>((floatPos.z * 2))
    );
    m_occupiedTiles.erase(oldPos);

    // 미니언 위치 업데이트 및 새로운 타일 점유 등록
    minion->SetPosition(newPos);
    m_occupiedTiles.emplace(newPos, instanceId);

    // 미니언 재배치(위치 변경) 알림 패킷 생성 및 브로드캐스트
    RelocateMinionNotify notify;
    notify.instanceId = minion->GetInstanceId();
    notify.x = x;
    notify.y = y;

    PacketWriter writer(
        PacketType::RelocateMinionNotify
    );

    writer.Write(notify);
    writer.Finish();

    Broadcast(
        writer.GetBuffer()
    );

    return ErrorCode::None;
}

void Room::WaveStart()
{
    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    m_gameState = GameState::Wave;
    m_waveManager.StartWave();
}

void Room::NotifySpawnEnemy(std::shared_ptr<Enemy> enemy)
{
    std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

    // Enemy Arrival 콜백 등록
    enemy->SetArrivalCallback([this](std::shared_ptr<Enemy> arrivedEnemy) {
        this->ArrivalEnemyHandle(arrivedEnemy);
        });

    // Enemy 생성 알림 패킷 생성 및 브로드캐스트
    SpawnEnemyNotify notify;
    notify.instanceId = enemy->GetInstanceId();
    notify.enemyId = enemy->GetId();
    notify.pathIndex = enemy->GetPathIndex();

    PacketWriter writer(
        PacketType::SpawnEnemyNotify
    );

    writer.Write(notify);
    writer.Finish();

    Broadcast(
        writer.GetBuffer()
    );
}

void Room::ArrivalEnemyHandle(std::shared_ptr<Enemy> enemy)
{
    if (!enemy)
        return;

    Logger::GetInstance().Info("Arrival Handle Start");
    m_hp -= enemy->GetDamage();

    // Enemy 생존 패킷 생성 및 브로드캐스트
    ArrivalEnemyNotify notify;
    notify.instanceId = enemy->GetInstanceId();
    notify.wallHp = m_hp;

    PacketWriter writer(
        PacketType::ArrivalEnemyNotify
    );

    writer.Write(notify);
    writer.Finish();

    Broadcast(
        writer.GetBuffer()
    );

    // Enemy 초기화 및 반환
    m_enemyManager.RemoveEnemy(
        enemy->GetInstanceId()
    );

    CheckEndWave();
    Logger::GetInstance().Info("Arrival Handle End");
}

void Room::DamageToEnemy(
    std::vector<std::shared_ptr<Enemy>> targets,
    std::shared_ptr<Minion> minion,
    int damage
)
{
    for (auto& enemy : targets)
    {
        if (!enemy || enemy->IsDead())
            continue;

        enemy->TakeDamage(damage);
        minion->OnHitEvents(shared_from_this(), enemy);

        // Enemy가 죽었을 경우 
        if (enemy->IsDead())
        {
            // Enemy Reward 지급
            PlayerAddGold(enemy->GetReward());
            minion->OnKillEvents(shared_from_this());

            // Enemy 초기화 및 반환
            m_enemyManager.RemoveEnemy(
                enemy->GetInstanceId()
            );

            CheckEndWave();
            BroadcastGoldUpdate();
        }
    }
}

// Projectile 공격 시 호출되는 DamageToEnemy
void Room::DamageToEnemy(
    std::shared_ptr<Enemy> target,
    std::shared_ptr<Minion> minion,
    std::shared_ptr<Projectile> projectile,
    int damage
)
{
    // AttackEvent 생성 및 큐에 추가
    AttackEvent atkEvent;

    atkEvent.attackType = AttackType::Default;
    atkEvent.isProjectile = true;
    atkEvent.minionInstanceId = minion->GetInstanceId();
    atkEvent.targets.push_back(target->GetInstanceId());
    atkEvent.damage = damage;

    m_attackQueue.push(atkEvent);

    target->TakeDamage(damage);
    minion->OnHitEvents(shared_from_this(), target);

    // Enemy가 죽었을 경우 
    if (target->IsDead())
    {
        // Enemy Reward 지급
        PlayerAddGold(target->GetReward());
        minion->OnKillEvents(shared_from_this());
        
        BroadcastGoldUpdate();

        // Enemy 초기화 및 반환
        m_enemyManager.RemoveEnemy(
            target->GetInstanceId()
        );

        CheckEndWave();
        BroadcastGoldUpdate();
    }
}

void Room::CheckEndWave()
{
    // Spawn이 끝나고 살아있는 Enemy가 없다면 Wave 종료
    if (m_waveManager.IsSpawningFinished() && m_enemyManager.GetAliveEnemyCount() <= 0)
    {
        // State 전환 및 Minion AC 초기화
        m_gameState = GameState::Intermission;
        m_minionManager.OnEndWave();

        EndWaveNotify notify;

        PacketWriter writer(
            PacketType::EndWaveNotify
        );

        writer.Write(notify);
        writer.Finish();

        Broadcast(
            writer.GetBuffer()
        );
    }
}

std::shared_ptr<Projectile> Room::SpawnProjectile(
    std::shared_ptr<Minion>& minion,
    std::shared_ptr<Enemy>& target,
    ProjectileHitType hitType
)
{
    if (!target)
        return nullptr;

    const Vector3 startPos = minion->GetPosition();
        
    std::shared_ptr<Projectile> projectile =
        m_projectileManager.SpawnProjectile(
            target,
            startPos,
            minion,
            minion->GetPower(),
            minion->GetProjectileSpeed(),
            minion->GetProjectileDuration(),
            minion->GetProjectileType() == ProjectileType::Chase,
            hitType
        );
        
    // Damage 콜백 설정
    projectile->SetAttackCallback(
        [this, projectile](
            std::shared_ptr<Enemy>& enemy,
            std::shared_ptr<Minion>& owner,
            int damage
            )
        {
            DamageToEnemy(
                enemy,
                owner,
                projectile,
                damage
            );
        }
    );

    // Return 콜백 설정
    projectile->SetReturnCallback(
        [this](
            uint32_t id
            )
        {
            m_projectileManager.RemoveProjectile(id);
        }
    );
    return projectile;
}

void Room::DefaultAttackHandle(
    uint32_t minionInstanceId,
    std::vector<std::shared_ptr<Enemy>> targets
)
{
    if (targets.empty())
    {
        return;
    }
        
    auto minion = m_minionManager.FindMinion(minionInstanceId);

    bool isProjectile = minion->GetDeliveryType() == DeliveryType::Projectile;

    // 공격 대상 Enemy의 InstanceId를 수집
    std::vector<uint32_t> targetIds;
    for (auto target : targets)
    {
        if (!target || target->IsDead())
        {
            continue;
        }
        targetIds.push_back(target->GetInstanceId());
    }

    // AttackEvent 생성 및 큐에 추가
    AttackEvent atkEvent;

    atkEvent.attackType = AttackType::Default;
    atkEvent.isProjectile = false;
    atkEvent.minionInstanceId = minionInstanceId;
    atkEvent.targets = targetIds;
    atkEvent.damage = minion->GetPower();

    m_attackQueue.push(atkEvent);

    std::vector<std::shared_ptr<Projectile>> projectiles;

    for (std::shared_ptr<Enemy> enemy : targets)
    {
        if (enemy)
        {
            // Projectile의 경우 설정
            if (isProjectile)
            {
                projectiles.push_back(
                    SpawnProjectile(
                        minion,
                        enemy,
                        minion->GetProjectileHitType()
                    )
                );
            }
        }
    }
    
    // Instant 공격
    if (!isProjectile)
    {
        DamageToEnemy(
            targets,
            minion,
            minion->GetPower()
        );
    };
}

void Room::EnemyTargetSkillHandle(
    uint32_t minionInstanceId,
    std::vector<std::shared_ptr<Enemy>> targets
)
{
    if (targets.empty())
        return;

    auto minion = m_minionManager.FindMinion(minionInstanceId);

	// 공격 대상 Enemy의 InstanceId를 수집
    std::vector<uint32_t> targetIds;
    for (auto target : targets)
    {
        if (!target || target->IsDead())
        {
            continue;
        }
        targetIds.push_back(target->GetInstanceId());
    }

	// AttackEvent 생성 및 큐에 추가
    AttackEvent atkEvent;

    atkEvent.attackType = AttackType::Default;
    atkEvent.isProjectile = false;
    atkEvent.minionInstanceId = minionInstanceId;
    atkEvent.targets = targetIds;
    atkEvent.damage = minion->GetPower();

    m_attackQueue.push(atkEvent);

	// EnemyTargetSkillStrategy 실행
    minion->GetEnemyTargetSkillStrategy()->Execute(
        minion,
        targets,
        *(minion->GetSkillData()),
        shared_from_this()
    );

}

void Room::MinionTargetSkillHandle(
    uint32_t minionInstanceId,
    std::vector<std::shared_ptr<Minion>> targets
)
{
    if (targets.empty())
        return;

    auto minion = m_minionManager.FindMinion(minionInstanceId);

    minion->GetMinionTargetSkillStrategy()->Execute(
        minion,
        targets,
        *(minion->GetSkillData()),
        shared_from_this()
    );

}

void Room::PlayerAddGold(int amount)
{
    std::vector<std::shared_ptr<Player>> players;

    {
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

        for (const auto& [id, player] : m_players)
        {
            players.push_back(player);
        }
    }

    for (const auto& player : players)
    {
        player->AddGold(amount);
    }
}

void Room::BroadcastAttack()
{
    std::vector<char> packet;

    {
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);

        if (m_attackQueue.empty())
            return;

	    Logger::GetInstance().Info("Broadcast Attack");

        PacketWriter writer(PacketType::AttackNotify);

        writer.Write(static_cast<int>(m_attackQueue.size()));

        while (!m_attackQueue.empty())
        {
            AttackEvent& atkEvent = m_attackQueue.front();

            writer.Write(atkEvent.attackType);
            writer.Write(atkEvent.isProjectile);
            writer.Write(atkEvent.minionInstanceId);
            writer.Write(atkEvent.damage);
            writer.Write(static_cast<int>(atkEvent.targets.size()));
            for (const auto& target : atkEvent.targets)
            {
                writer.Write(target);
            }

            m_attackQueue.pop();
        }

        writer.Finish();
        packet = writer.GetBuffer();
    }

    Broadcast(packet);
}