#pragma once

#include <memory>

#include "Game/Pool/ObjectPool.h"

class Minion;
class Enemy;
class Projectile;

class ObjectPoolManager
{
public:

	ObjectPoolManager();

    std::shared_ptr<Minion> AcquireMinion();

    void ReleaseMinion(std::shared_ptr<Minion> minion);

    std::shared_ptr<Enemy> AcquireEnemy();

    void ReleaseEnemy(std::shared_ptr<Enemy> enemy);

    std::shared_ptr<Projectile> AcquireProjectile();

    void ReleaseProjectile(std::shared_ptr<Projectile> projectile);

private:

    std::unique_ptr<ObjectPool<Minion>> m_minionPool;
    std::unique_ptr<ObjectPool<Enemy>> m_enemyPool;
    std::unique_ptr<ObjectPool<Projectile>> m_projectilePool;
};