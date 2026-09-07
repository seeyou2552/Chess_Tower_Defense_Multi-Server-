#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <mutex>
#include <queue>

#include "Game/GameInstance.h"
#include "Game/GameState.h"
#include "Game/Stage/StageData.h"
#include "Game/Stage/StageTileData.h"
#include "Network/ErrorCode.h"
#include "Game/Math/Vector3.h"
#include "Game/Skill/Projectile.h"
#include "Network/Packet/MinionPacket.h"
#include "Game/Minion/MinionStat.h"
#include "Game/Skill/SkillTypes.h"

#include "Game/WaveManager.h"
#include "Game/Minion/MinionManager.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Skill/ProjectileManager.h"

class GameInstance;
class Player;
class SpawnManager;
class TickManager;
class Minion;

class Room
    : public std::enable_shared_from_this<Room>
{
public:

    explicit Room(
        uint32_t id,
        TickManager& tickManager,
        SpawnManager& spawnManager,
		std::shared_ptr<StageData> stageData,
		std::shared_ptr<StageTileData> stageTileData
    );

    void Update(float deltaTime);

    void EnqueueCommand(
        std::function<void()> command
    );

    void ProcessCommands();

    uint32_t GetId() const
    {
        return m_id;
    }

	uint32_t GetStageId() const
	{
		return m_stageData->stageId;
	}

    uint64_t GetTimestamp();

    void Enter(
        std::shared_ptr<Player> player
    );

    void Leave(
        std::shared_ptr<Player> player
    );

    void Broadcast( const std::vector<char>& packet);

    void BroadcastGoldUpdate();

	bool IsFull() const
	{
        std::lock_guard<std::recursive_mutex> lock(m_roomMutex);
		return m_players.size() >= 2;
	}

    std::shared_ptr<GameInstance> GetGame()
    {
        return m_game;
    }

    void SetGame( std::shared_ptr<GameInstance> game )
    {
        m_game = game;
    }

    MinionManager& GetMinionManager()
    {
        return m_minionManager;
    }

    EnemyManager& GetEnemyManager()
    {
        return m_enemyManager;
    }

    std::vector<std::shared_ptr<Player>> GetPlayersSnapshot() const;

    void StageStart();

    bool PlayerReadyCheck(std::shared_ptr<Player> player);

    ErrorCode SpawnMinion(
        uint32_t minionId,
        std::shared_ptr<Player>& player,
        int x,
        int y
    );

    void ReturnMinion(uint32_t instanceId);

    ErrorCode RelocateMinion(
        uint32_t instanceId,
        uint32_t orderId,
        int x,
        int y
    );

    void WaveStart();

    void NotifySpawnEnemy(std::shared_ptr<Enemy> enemy);

    void ArrivalEnemyHandle(std::shared_ptr<Enemy> enemy);

    void DamageToEnemy(
        std::vector<std::shared_ptr<Enemy>> targets,
        std::shared_ptr<Minion> minion,
        int damage
    );

    void DamageToEnemy(
        std::shared_ptr<Enemy> target,
        std::shared_ptr<Minion> minion,
		std::shared_ptr<Projectile> projectile,
        int damage
    );

    void DefaultAttackHandle(
        uint32_t minionInstanceId,
        std::vector<std::shared_ptr<Enemy>> targets
    );

    void EnemyTargetSkillHandle(
        uint32_t minionInstanceId,
        std::vector<std::shared_ptr<Enemy>> targets
    );

    void MinionTargetSkillHandle(
        uint32_t minionInstanceId,
        std::vector<std::shared_ptr<Minion>> targets
    );

    void CheckEndWave();

    std::shared_ptr<Projectile> SpawnProjectile(
        std::shared_ptr<Minion>& minion,
        std::shared_ptr<Enemy>& target,
        ProjectileHitType hitType
    );

    void PlayerAddGold(int amount);

    void BroadcastAttack();

private:

    std::mutex m_commandMutex;
    std::queue<std::function<void()>> m_commandQueue;

    mutable std::recursive_mutex m_roomMutex;


    std::unordered_map<
        uint32_t,
        std::shared_ptr<Player>
    > m_players;

    uint32_t m_id;

    std::shared_ptr<GameInstance> m_game;
    SpawnManager& m_spawnManager;
    TickManager& m_tickManager;

	std::shared_ptr<StageData> m_stageData;
    std::shared_ptr<StageTileData> m_stageTileData;

    std::unordered_map<uint32_t, bool> m_playerReady;

    WaveManager m_waveManager;

    EnemyManager m_enemyManager;
    MinionManager m_minionManager;
    ProjectileManager m_projectileManager;

    // 타일에 이미 다른 미니언이 스폰되어 있는지 관리하는 점유 상태 맵
    std::unordered_map<Vector3Int, uint64_t, Vector3IntHash> m_occupiedTiles;

    int m_hp = 100;
    GameState m_gameState;

    float m_syncTimer = 0.0f;

    std::queue<AttackEvent> m_attackQueue;


};