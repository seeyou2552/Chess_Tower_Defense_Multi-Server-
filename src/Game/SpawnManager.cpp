#include "Game/SpawnManager.h"

#include "Game/Pool/ObjectPoolManager.h"
#include "Game/Minion/Minion.h"
#include "Game/Minion/MinionDataManager.h"
#include "Game/Minion/MinionData.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Enemy/EnemyDataManager.h"
#include "Game/Enemy/EnemyData.h"
#include "Game/Room.h"
#include "Game/Skill/Projectile.h"

#include "Core/Logger.h"

SpawnManager::SpawnManager(
    ObjectPoolManager& poolManager
)
    :
    m_poolManager(poolManager)
{
}

std::shared_ptr<Minion>
SpawnManager::SpawnMinion(
    uint32_t instanceId,
    uint32_t orderId,
    uint32_t minionId,
    int x,
    int y
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const MinionData* data = MinionDataManager::GetInstance().GetData(minionId);
    
    if (!data)
    {
        return nullptr;
    }

    auto minion =
        m_poolManager.AcquireMinion();

    minion->Init(
        instanceId,
        orderId,
        *data,
        x,
        y
    );

    return minion;
}

std::shared_ptr<Enemy>
SpawnManager::SpawnEnemy(
    uint32_t instanceId,
    uint32_t enemyId,
    int x,
    int y
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const EnemyData* data =
        EnemyDataManager::GetInstance().GetData(enemyId);

    if (!data)
    {
        Logger::GetInstance().Error(
            "EnemyData not found : "
            + std::to_string(enemyId)
        );

        return nullptr;
    }

    auto enemy =
        m_poolManager.AcquireEnemy();

    if (!enemy)
    {
        Logger::GetInstance().Error(
            "AcquireEnemy returned nullptr"
        );

        return nullptr;
    }

    enemy->Init(
        instanceId,
        *data,
        x,
        y
    );

    return enemy;
}

std::shared_ptr<Projectile> SpawnManager::SpawnProjectile(
    uint32_t instanceId,
    std::shared_ptr<Enemy>& target,
    std::shared_ptr<Minion>& minion,
    Vector3 startPos,
    int damage,
    float speed,
    float activeDuration,
    bool isChase,
    ProjectileHitType hitType,
    EnemyManager& enemyManager
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto projectile =
        m_poolManager.AcquireProjectile();

    if (!projectile)
    {
        Logger::GetInstance().Error(
            "AcquireProjectile returned nullptr"
        );

        return nullptr;
    }

    projectile->Init(
        instanceId,
        target,
        startPos,
        minion,
        damage,
        speed,
        activeDuration,
        isChase,
        hitType,
        enemyManager
    );

    return projectile;
}

void SpawnManager::ReleaseMinion(
    std::shared_ptr<Minion> minion
)
{
    if (!minion)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_poolManager.ReleaseMinion(
        minion
    );
}

void SpawnManager::ReleaseEnemy(
    std::shared_ptr<Enemy> enemy
)
{
    if (!enemy)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_poolManager.ReleaseEnemy(
        enemy
    );
}

void SpawnManager::ReleaseProjectile(
    std::shared_ptr<Projectile> projectile
)
{
    if (!projectile)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_poolManager.ReleaseProjectile(
        projectile
    );
}