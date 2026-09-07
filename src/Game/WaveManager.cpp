#include "Game/WaveManager.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Room.h"
#include "Game/Math/Vector3.h"

#include "Core/Logger.h"

WaveManager::WaveManager(
    Room& room,
    EnemyManager& enemyManager,
    std::shared_ptr<StageData> stageData
)
    :
    m_room(room),
    m_enemyManager(enemyManager)
{
    m_stageData = stageData;

    m_currentWave = 0;
    m_enemyListIndex = 0;
    m_remainingSpawnCount = 0;

    m_spawnElapsedTime = 0.0f;
    m_aliveEnemyCount = 0;

    m_waveStarted = false;
    m_spawnFinished = true;
    m_waveFinished = false;
}

void WaveManager::Update(
    float deltaTime
)
{
    if (!m_waveStarted)
        return;

    if (m_spawnFinished)
        return;

    m_spawnElapsedTime += deltaTime;

    if (m_enemyListIndex >=
        m_stageData->waves[m_currentWave - 1].enemyList.size())
    {
        m_spawnFinished = true;
        return;
    }

    auto& enemyData =
        m_stageData
        ->waves[m_currentWave - 1]
        .enemyList[m_enemyListIndex];

    if (m_remainingSpawnCount == 0)
    {
        m_remainingSpawnCount =
            enemyData.count;
    }

    if (m_spawnElapsedTime < enemyData.spawnDelay)
        return;

    m_spawnElapsedTime = 0.0f;

    SpawnNextEnemy();
}

void WaveManager::StartWave()
{
    if (!m_stageData)
    {
        Logger::GetInstance().Error("No Stage Data");
        return;
    }
        

    if (m_currentWave >= m_stageData->waves.size())
    {
        Logger::GetInstance().Warning(
            "No More Wave"
        );

        return;
    }

    ++m_currentWave;

    m_enemyListIndex = 0;
    m_remainingSpawnCount = 0;

    m_spawnElapsedTime = 0.0f;

    m_waveStarted = true;
    m_spawnFinished = false;
    m_waveFinished = false;
}

void WaveManager::SpawnNextEnemy()
{

    if (!m_stageData)
    {
        Logger::GetInstance().Error("SpawnNextEnemy : StageData nullptr");
        return;
    }

    if (m_currentWave == 0)
    {
        Logger::GetInstance().Error("CurrentWave is 0");
        return;
    }

    auto& wave =
        m_stageData
        ->waves[m_currentWave - 1];

    if (m_enemyListIndex >= wave.enemyList.size())
    {
        m_spawnFinished = true;
        return;
    }

    Logger::GetInstance().Info(
        "EnemyListIndex : "
        + std::to_string(m_enemyListIndex)
    );

    if (m_stageData->paths.empty())
    {
        Logger::GetInstance().Error(
            "Stage has no path"
        );
        return;
    }

    auto& spawnData =
        wave.enemyList[m_enemyListIndex];

    std::uniform_int_distribution<size_t> pathDistribution(
        0,
        m_stageData->paths.size() - 1
    );

    size_t pathIndex =
        pathDistribution(m_randomEngine);

    const PathData& path =
        m_stageData->paths[pathIndex];

    if (path.waypoints.empty())
    {
        Logger::GetInstance().Error(
            "Selected path has no waypoint"
        );
        return;
    }

    const Vector3& spawnPosition =
        path.waypoints[0];

    std::shared_ptr<Enemy> enemy =
        m_enemyManager.SpawnEnemy(
            spawnData.enemyId,
            spawnPosition.x,
            spawnPosition.y
        );

    if (!enemy)
    {
        Logger::GetInstance().Error(
            "Enemy Spawn Failed : "
            + std::to_string(spawnData.enemyId)
        );
        return;
    }

    enemy->SetPath(
        static_cast<uint32_t>(pathIndex),
        path.waypoints
    );

    --m_remainingSpawnCount;

    ++m_aliveEnemyCount;

    if (m_remainingSpawnCount == 0)
    {
        ++m_enemyListIndex;
    }

    if (m_enemyListIndex >= wave.enemyList.size())
    {
        m_spawnFinished = true;

        Logger::GetInstance().Info(
            "Enemy Spawn Finished : Wave "
            + std::to_string(m_currentWave)
        );
    }

    m_room.NotifySpawnEnemy(enemy);

}