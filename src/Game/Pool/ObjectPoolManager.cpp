#include "Game/Pool/ObjectPoolManager.h"

#include "Game/Minion/Minion.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Skill/Projectile.h"


ObjectPoolManager::ObjectPoolManager()
{
    m_minionPool =
        std::make_unique<
        ObjectPool<Minion>
        >(
            []()
            {
                return std::make_shared<Minion>();
            }
        );

    m_enemyPool =
        std::make_unique<
        ObjectPool<Enemy>
        >(
            []()
            {
                return std::make_shared<Enemy>();
            }
        );

    m_projectilePool =
        std::make_unique<
        ObjectPool<Projectile>
        >(
            []()
            {
                return std::make_shared<Projectile>();
            }
        );
}

std::shared_ptr<Minion>
ObjectPoolManager::AcquireMinion()
{
    return m_minionPool->Acquire();
}

void ObjectPoolManager::ReleaseMinion(
    std::shared_ptr<Minion> minion
)
{
    m_minionPool->Release(
        minion
    );
}

std::shared_ptr<Enemy>
ObjectPoolManager::AcquireEnemy()
{
    return m_enemyPool->Acquire();
}

void ObjectPoolManager::ReleaseEnemy(
    std::shared_ptr<Enemy> enemy
)
{
    m_enemyPool->Release(
        enemy
    );
}

std::shared_ptr<Projectile>
ObjectPoolManager::AcquireProjectile()
{
    return m_projectilePool->Acquire();
}

void ObjectPoolManager::ReleaseProjectile(
    std::shared_ptr<Projectile> projectile
)
{
    m_projectilePool->Release(
        projectile
    );
}