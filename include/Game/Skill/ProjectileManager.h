#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>

#include "Game/Math/Vector3.h"
#include "Game/Skill/SkillTypes.h"

class Projectile;
class SpawnManager;
class EnemyManager;
class Enemy;
class Minion;

class ProjectileManager
{
public:

    explicit ProjectileManager(
        SpawnManager& spawnManager,
        EnemyManager& enemyManager
    );

    std::shared_ptr<Projectile> SpawnProjectile(
        std::shared_ptr<Enemy>& target,
        Vector3 startPos,
        std::shared_ptr<Minion>& minion,
        int damage,
        float speed,
        float activeDuration,
        bool isChase,
        ProjectileHitType hitType
    );

    void RemoveProjectile(uint32_t id);

    void Update(float deltaTime);

private:

    SpawnManager& m_spawnManager;
    EnemyManager& m_enemyManager;

    uint32_t m_nextInstanceId = 1;

    std::unordered_map<
        uint32_t,
        std::shared_ptr<Projectile>
    > m_projectiles;
};