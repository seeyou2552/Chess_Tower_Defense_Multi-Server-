#include "Game/Enemy/EnemyManager.h"
#include "Game/Enemy/Enemy.h"
#include "Game/SpawnManager.h"
#include "Game/Math/Vector3.h"

#include <algorithm>
#include <mutex>

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
    std::shared_ptr<Enemy> enemy = m_spawnManager.SpawnEnemy(
        m_nextInstanceId,
        enemyId,
        x,
        y
    );

    if (!enemy)
        return nullptr;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);
        m_enemies.emplace(m_nextInstanceId, enemy);
        m_nextInstanceId++;
    }

    return enemy;
}

void EnemyManager::RemoveEnemy(uint32_t instanceId)
{
    std::lock_guard<std::mutex> lock(m_enemyMutex);
    m_removeQueue.push_back(instanceId);
}

void EnemyManager::Update(float deltaTime)
{
    // Update 중에는 락 범위를 잡거나, 복사해서 처리하는 것이 안전합니다.

    for (auto& [id, enemy] : m_enemies)
    {
        if (enemy)
        {
            enemy->Update(deltaTime);
        }
    }

    for (uint32_t id : m_removeQueue)
    {
        auto iter = m_enemies.find(id);
        if (iter == m_enemies.end())
            continue;

        auto enemy = iter->second;
        m_enemies.erase(iter);

        m_spawnManager.ReleaseEnemy(enemy);
    }

    m_removeQueue.clear();
}

int EnemyManager::GetAliveEnemyCount() const
{
    std::lock_guard<std::mutex> lock(m_enemyMutex);
    if (m_enemies.size() < m_removeQueue.size())
        return 0;
    return static_cast<int>(m_enemies.size() - m_removeQueue.size());
}

std::vector<std::shared_ptr<Enemy>> EnemyManager::FindEnemyInRange(
    const Vector3& position,
    float range,
    int maxTargets
) const
{
    const float rangeSquared = range * range;
    std::vector<std::shared_ptr<Enemy>> enemies;

    {
        std::lock_guard<std::mutex> lock(m_enemyMutex);

        for (const auto& [id, enemy] : m_enemies)
        {
            if (!enemy || !enemy->IsActive())
                continue;

            Vector3 enemyPos = enemy->GetPosition();
            const float dx = enemyPos.x - position.x;
            const float dy = enemyPos.y - position.y;
            const float distanceSquared = (dx * dx) + (dy * dy);

            if (distanceSquared <= rangeSquared)
            {
                enemies.push_back(enemy);
            }
        }
    }

    std::sort(
        enemies.begin(),
        enemies.end(),
        [&position](const std::shared_ptr<Enemy>& lhs, const std::shared_ptr<Enemy>& rhs)
        {
            const Vector3& lPos = lhs->GetPosition();
            const Vector3& rPos = rhs->GetPosition();

            const float lDistSq =
                (lPos.x - position.x) * (lPos.x - position.x) +
                (lPos.y - position.y) * (lPos.y - position.y);

            const float rDistSq =
                (rPos.x - position.x) * (rPos.x - position.x) +
                (rPos.y - position.y) * (rPos.y - position.y);

            // 거리가 다르면 가까운 적 우선
            if (lDistSq != rDistSq)
                return lDistSq < rDistSq;

            // 거리가 같으면 instanceId가 작은 적 우선
            return lhs->GetInstanceId() < rhs->GetInstanceId();
        }
    );

    if (maxTargets > 0 && static_cast<int>(enemies.size()) > maxTargets)
    {
        enemies.resize(static_cast<size_t>(maxTargets));
    }

    return enemies;
}

std::vector<std::shared_ptr<Enemy>>
EnemyManager::GetActiveEnemiesSnapshot() const
{
    std::vector<std::shared_ptr<Enemy>> enemies;

    std::lock_guard<std::mutex> lock(m_enemyMutex);

    enemies.reserve(m_enemies.size());

    for (const auto& [id, enemy] : m_enemies)
    {
        if (!enemy)
            continue;

        if (!enemy->IsActive() || enemy->IsDead())
            continue;

        enemies.push_back(enemy);
    }

    return enemies;
}