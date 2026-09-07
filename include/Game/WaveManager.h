#pragma once

#include <cstdint>
#include <memory>
#include <random>

#include "Game/Stage/StageData.h"

class Room;
class EnemyManager;

class WaveManager
{
public:
    explicit WaveManager(
        Room& room,
        EnemyManager& enemyManager,
        std::shared_ptr<StageData> stageData
    );

    void Update(
        float deltaTime
    );

    void StartWave();

    bool IsWaveFinished() const
    {
        return m_waveFinished;
    }

    bool IsSpawningFinished() const
    {
        return m_spawnFinished;
    }

    uint32_t GetCurrentWave() const
    {
        return m_currentWave;
    }

    uint32_t GetAliveEnemyCount() const
    {
        return m_aliveEnemyCount;
    }

    void OnEnemySpawned()
    {
        ++m_aliveEnemyCount;
    }

    void OnEnemyDead();

private:
    void SpawnNextEnemy();

private:
    Room& m_room;
    EnemyManager& m_enemyManager;

    std::shared_ptr<StageData> m_stageData;

    std::mt19937 m_randomEngine{
        std::random_device{}()
    };

    uint32_t m_currentWave = 0;

    size_t m_enemyListIndex = 0;
    uint32_t m_remainingSpawnCount = 0;

    float m_spawnElapsedTime = 0.0f;

    uint32_t m_aliveEnemyCount = 0;

    bool m_waveStarted = false;
    bool m_spawnFinished = false;
    bool m_waveFinished = false;
    
};