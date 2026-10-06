#include "Game/Enemy/EnemyManager.h"
#include "Game/Enemy/Enemy.h"
#include "Game/SpawnManager.h"
#include "Game/Math/Vector3.h"

#include <algorithm>
#include <mutex>
#include <utility>

#include "Core/Logger.h"

EnemyManager::EnemyManager
(
    SpawnManager& spawnManager
)
    :
    m_spawnManager(spawnManager)
{

}

std::shared_ptr<Enemy> EnemyManager::SpawnEnemy(uint32_t enemyId, int x, int y)
{
    uint32_t instanceId;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);
        instanceId = m_nextInstanceId++;
    }

    std::shared_ptr<Enemy> enemy = m_spawnManager.SpawnEnemy(
        instanceId,
        enemyId,
        x,
        y
    );

    if (!enemy)
        return nullptr;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);
        m_enemies.emplace(instanceId, enemy);
    }

    return enemy;
}

void EnemyManager::RemoveEnemy(uint32_t instanceId)
{
    std::lock_guard<std::mutex> lock(m_enemyMutex);
    m_removeQueue.push_back(instanceId);
}

void EnemyManager::Clear()
{
    std::vector<std::shared_ptr<Enemy>> enemies;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);

        enemies.reserve(m_enemies.size());

        for (auto& [id, enemy] : m_enemies)
        {
            if (enemy)
                enemies.push_back(std::move(enemy));
        }

        m_enemies.clear();
        m_removeQueue.clear();
    }

    for (const auto& enemy : enemies)
    {
        m_spawnManager.ReleaseEnemy(enemy);
    }
}

void EnemyManager::Update(float deltaTime)
{
    // 1. 현재 Enemy 목록을 snapshot으로 복사
    std::vector<std::shared_ptr<Enemy>> enemies;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);

        enemies.reserve(m_enemies.size());

        for (const auto& [id, enemy] : m_enemies)
        {
            if (enemy)
                enemies.push_back(enemy);
        }
    }

    // 2. 실제 Enemy Update는 lock 없이 수행
    for (const auto& enemy : enemies)
    {
        if (enemy)
            enemy->Update(deltaTime);
    }

    // 3. 제거 대상 snapshot
    std::vector<std::shared_ptr<Enemy>> removeEnemies;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);

        for (uint32_t id : m_removeQueue)
        {
            auto iter = m_enemies.find(id);

            if (iter == m_enemies.end())
                continue;

            if (iter->second)
                removeEnemies.push_back(iter->second);

            m_enemies.erase(iter);
        }

        m_removeQueue.clear();
    }

    // 4. Pool 반환은 lock 밖에서 수행
    for (const auto& enemy : removeEnemies)
    {
        m_spawnManager.ReleaseEnemy(enemy);
    }
}

int EnemyManager::GetAliveEnemyCount() const
{
    std::lock_guard<std::mutex> lock(m_enemyMutex);

    if (m_enemies.size() < m_removeQueue.size())
        return 0;

    return static_cast<int>(
        m_enemies.size() - m_removeQueue.size()
        );
}

std::vector<std::shared_ptr<Enemy>>
EnemyManager::FindEnemyInRange(
    const Vector3& position,
    float range,
    int maxTargets
) const
{
    const float rangeSquared = range * range;

    struct EnemyCandidate
    {
        std::shared_ptr<Enemy> enemy;
        float distanceSquared;
        uint32_t instanceId;
    };

    std::vector<EnemyCandidate> candidates;
    std::vector<std::pair<uint32_t, std::shared_ptr<Enemy>>> enemiesSnapshot;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);

        for (const auto& [id, enemy] : m_enemies)
        {
            enemiesSnapshot.emplace_back(id, enemy);
        }
    }

    candidates.reserve(enemiesSnapshot.size());

    for (const auto& [id, enemy] : enemiesSnapshot)
    {
        if (!enemy || !enemy->IsActive())
            continue;

        const Vector3 enemyPos = enemy->GetPosition();

        const float dx = enemyPos.x - position.x;
        const float dy = enemyPos.y - position.y;

        const float distanceSquared =
            (dx * dx) + (dy * dy);

        if (distanceSquared <= rangeSquared)
        {
            candidates.push_back(
                {
                    enemy,
                    distanceSquared,
                    id
                }
            );
        }
    }

    // Enemy의 Position을 다시 읽지 않고
    // snapshot 당시 계산한 거리로 정렬
    std::sort(
        candidates.begin(),
        candidates.end(),
        [](const EnemyCandidate& lhs, const EnemyCandidate& rhs)
        {
            if (lhs.distanceSquared != rhs.distanceSquared)
                return lhs.distanceSquared < rhs.distanceSquared;

            return lhs.instanceId < rhs.instanceId;
        }
    );

    if (maxTargets > 0 &&
        static_cast<int>(candidates.size()) > maxTargets)
    {
        candidates.resize(
            static_cast<size_t>(maxTargets)
        );
    }

    std::vector<std::shared_ptr<Enemy>> enemies;
    enemies.reserve(candidates.size());

    for (const auto& candidate : candidates)
    {
        enemies.push_back(candidate.enemy);
    }

    return enemies;
}

std::vector<std::shared_ptr<Enemy>>
EnemyManager::GetActiveEnemiesSnapshot() const
{
    std::vector<std::shared_ptr<Enemy>> enemies;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);

        enemies.reserve(m_enemies.size());

        for (const auto& [id, enemy] : m_enemies)
        {
            if (enemy)
                enemies.push_back(enemy);
        }
    }

    enemies.erase(
        std::remove_if(
            enemies.begin(),
            enemies.end(),
            [](const std::shared_ptr<Enemy>& enemy)
            {
                return !enemy->IsActive() || enemy->IsDead();
            }
        ),
        enemies.end()
    );

    return enemies;
}