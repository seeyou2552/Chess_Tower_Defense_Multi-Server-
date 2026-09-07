#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <mutex>

#include "Game/Math/Vector3.h"

class EnemyData;
class Enemy;
class SpawnManager;

class EnemyManager
{
public:

    explicit EnemyManager
    (
        SpawnManager& spawnManager
    );

    std::shared_ptr<Enemy> SpawnEnemy(
        uint32_t enemyId,
        int x,
        int y
    );

    void RemoveEnemy(uint32_t instanceId);

    void Update( float deltaTime );

    int GetAliveEnemyCount() const;

    std::vector<std::shared_ptr<Enemy>> FindEnemyInRange(
        const Vector3& position,
        float range,
        int maxTargets
    ) const;

    std::vector<std::shared_ptr<Enemy>> GetActiveEnemiesSnapshot() const;


private:

    mutable std::mutex m_enemyMutex;
    
    SpawnManager& m_spawnManager;

    uint32_t m_nextInstanceId = 1;

    std::unordered_map<
        uint32_t,
        std::shared_ptr<Enemy>
    > m_enemies;

    std::vector<uint32_t> m_removeQueue;
};