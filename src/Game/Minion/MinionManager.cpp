#include "Game/Minion/MinionManager.h"
#include "Game/Minion/Minion.h"
#include "Game/Minion/MinionData.h"
#include "Game/SpawnManager.h"
#include "Game/Enemy/EnemyManager.h"

#include "Core/Logger.h"

#include <mutex>

MinionManager::MinionManager
(
    SpawnManager& spawnManager,
    EnemyManager& enemyManager
)
    :
    m_spawnManager(spawnManager),
    m_enemyManager(enemyManager)
{

}

std::shared_ptr<Minion> MinionManager::SpawnMinion(
    uint32_t minionId,
    uint32_t orderId,
    int x,
    int y
)
{
    uint32_t instanceId;

    {
        std::lock_guard<std::mutex> lock(m_minionMutex);
        instanceId = m_nextInstanceId++;
    }

    std::shared_ptr<Minion> minion = m_spawnManager.SpawnMinion(
        instanceId,
        orderId,
        minionId,
        x,
        y
    );

    if (!minion)
        return nullptr;

    {
        std::lock_guard<std::mutex> lock(m_minionMutex);
        m_minions.emplace(
            instanceId,
            minion
        );
    }

    return minion;
}

void MinionManager::RemoveMinion(uint32_t instanceId)
{
    std::shared_ptr<Minion> minion;

    {
        std::lock_guard<std::mutex> lock(m_minionMutex);
        auto iter = m_minions.find(instanceId);
        if (iter == m_minions.end())
            return;

        minion = iter->second;
        m_minions.erase(iter);
    }

    m_spawnManager.ReleaseMinion(minion);
}

void MinionManager::Update(
    float deltaTime
)
{
    std::vector<std::shared_ptr<Minion>> minions;

    {
        std::lock_guard<std::mutex> lock(m_minionMutex);
        minions.reserve(m_minions.size());
        for (const auto& [id, minion] : m_minions)
        {
            if (minion)
                minions.push_back(minion);
        }
    }

    for (const auto& minion : minions)
    {
        minion->Update(deltaTime);

        if (minion->AttackReadyCheck())
        {
            TargetType type = minion->GetTargetType();

            switch (type)
            {
                case TargetType::Enemy:
                {
                    std::vector<std::shared_ptr<Enemy>> enemies =
                        m_enemyManager.FindEnemyInRange(
                            minion->GetPosition(),
                            minion->GetAttackRange(),
                            minion->GetMaxTargets()
                        );

                    if (enemies.empty())
                    {
                        continue;
                    }
                        

                    minion->OnEnemyTarget(enemies);
                }
                break;

                case TargetType::Minion:
                {
                    std::vector<std::shared_ptr<Minion>> minions =
                        FindMinionInRange(
                            minion->GetPosition(),
                            minion->GetAttackRange(),
                            minion->GetMaxTargets()
                        );

                    if (minions.empty())
                        continue;

                    minion->OnMinionTarget(minions);
                }
                break;
            }
            
        }
    }
}

void MinionManager::OnEndWave()
{
    std::vector<std::shared_ptr<Minion>> minions;

    {
        std::lock_guard<std::mutex> lock(m_minionMutex);
        for (const auto& [id, minion] : m_minions)
        {
            if (minion)
                minions.push_back(minion);
        }
    }

    for (const auto& minion : minions)
    {
        minion->ResetAC();
        minion->ClearBuffs();
    }
}

std::vector<std::shared_ptr<Minion>> MinionManager::FindMinionInRange(
    const Vector3& position,
    float range,
    int maxTargets
) const
{
    const float rangeSquared = range * range;
    std::vector<std::shared_ptr<Minion>> minions;

    {
        std::lock_guard<std::mutex> lock(m_minionMutex);

        for (const auto& [id, minion] : m_minions)
        {
            if (!minion)
                continue;

            Vector3 minionPos = minion->GetPosition();
            const float dx = minionPos.x - position.x;
            const float dy = minionPos.y - position.y;
            const float distanceSquared = (dx * dx) + (dy * dy);

            if (distanceSquared <= rangeSquared)
            {
                minions.push_back(minion);
            }
        }
    }

    // 범위 내에 찾은 적들을 거리 오름차순(가장 가까운 순)으로 정렬
    std::sort(
        minions.begin(),
        minions.end(),
        [&position](const std::shared_ptr<Minion>& lhs, const std::shared_ptr<Minion>& rhs)
        {
            const Vector3& lPos = lhs->GetPosition();
            const Vector3& rPos = rhs->GetPosition();

            float lDistSq = (lPos.x - position.x) * (lPos.x - position.x) + (lPos.y - position.y) * (lPos.y - position.y);
            float rDistSq = (rPos.x - position.x) * (rPos.x - position.x) + (rPos.y - position.y) * (rPos.y - position.y);

            return lDistSq < rDistSq;
        }
    );

    // maxTargets가 지정되어 있고, 찾은 적이 그보다 많다면 개수 제한
    if (maxTargets > 0 && static_cast<int>(minions.size()) > maxTargets)
    {
        minions.resize(static_cast<size_t>(maxTargets));
    }

    return minions;
}