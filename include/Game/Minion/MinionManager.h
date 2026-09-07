#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <mutex>

#include "Game/Skill/SkillTypes.h"
#include "Game/Math/Vector3.h"

class MinionData;
class Minion;
class SpawnManager;
class EnemyManager;

class MinionManager
{
public:

    explicit MinionManager(
        SpawnManager& spawnManager,
        EnemyManager& enemyManager
    );

    std::shared_ptr<Minion> SpawnMinion(
        uint32_t minionId,
        uint32_t orderId,
        int x,
        int y
    );

    void RemoveMinion(uint32_t instanceId);

    void Update(float deltaTime);

    std::shared_ptr<Minion> FindMinion(uint32_t instanceId)
    {
        std::lock_guard<std::mutex> lock(m_minionMutex);
        auto it = m_minions.find(instanceId);

        // 찾았다면 해당 shared_ptr 반환
        if (it != m_minions.end())
        {
            return it->second;
        }

        // 못 찾았을 경우 nullptr 반환
        return nullptr;
    };

    void OnEndWave();

    std::vector<std::shared_ptr<Minion>> FindMinionInRange(
        const Vector3& position,
        float range,
        int maxTargets
    ) const;

private:

    mutable std::mutex m_minionMutex;
    SpawnManager& m_spawnManager;
    EnemyManager& m_enemyManager;

    uint32_t m_nextInstanceId = 1;

    std::unordered_map<
        uint32_t,
        std::shared_ptr<Minion>
    > m_minions;
};