#pragma once

#include <cstdint>
#include <memory>
#include <mutex>

#include "Game/Math/Vector3.h"
#include "Game/Skill/SkillTypes.h"

class Room;
class Minion;
class MinionData;
class MinionDataManager;
class Enemy;
class EnemyManager;
class EnemyDataManager;
class ObjectPoolManager;
class Projectile;

class SpawnManager
{
public:

    SpawnManager(
        ObjectPoolManager& poolManager
    );

    std::shared_ptr<Minion> SpawnMinion(
        uint32_t instanceId,
        uint32_t orderId,
        uint32_t minionId,
        int x,
        int y
    );

    void ReleaseMinion(std::shared_ptr<Minion> minion);

    std::shared_ptr<Enemy> SpawnEnemy(
        uint32_t instanceId,
        uint32_t enemyId,
        int x,
        int y
    );

    void ReleaseEnemy(std::shared_ptr<Enemy> enemy);

    std::shared_ptr<Projectile> SpawnProjectile(
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
    );

    void ReleaseProjectile(std::shared_ptr<Projectile> projectile);

private:

    std::mutex m_mutex;
    uint64_t m_nextInstanceId = 1;

    ObjectPoolManager& m_poolManager;
};