#include "Game/Skill/ProjectileManager.h"
#include "Game/Skill/Projectile.h"
#include "Game/SpawnManager.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Minion/Minion.h"
#include "Game/Enemy/Enemy.h"

#include "Core/Logger.h"

ProjectileManager::ProjectileManager(
    SpawnManager& spawnManager,
    EnemyManager& enemyManager
)
    :
    m_spawnManager(spawnManager),
    m_enemyManager(enemyManager)
{
}

std::shared_ptr<Projectile> ProjectileManager::SpawnProjectile(
    std::shared_ptr<Enemy>& target,
    Vector3 startPos,
    std::shared_ptr<Minion>& minion,
    int damage,
    float speed,
    float activeDuration,
    bool isChase,
    ProjectileHitType hitType
)
{
    auto projectile = m_spawnManager.SpawnProjectile(
        m_nextInstanceId,
        target,
        minion,
        startPos,
        damage,
        speed,
        activeDuration,
        isChase,
        hitType,
        m_enemyManager
    );

    if (!projectile)
        return nullptr;

    m_projectiles.emplace(
        m_nextInstanceId,
        projectile
    );

    m_nextInstanceId++;

    return projectile;
}

void ProjectileManager::RemoveProjectile(uint32_t id)
{

    std::shared_ptr<Projectile> projectile = m_projectiles[id];
    if (!projectile) return;

    m_projectiles.erase(id);

    m_spawnManager.ReleaseProjectile(projectile);
    Logger::GetInstance().Info("Projectile Return");
}

void ProjectileManager::Update(float deltaTime)
{
    // 순회 중 투사체가 삭제되더라도 꼬이지 않도록 ID 목록을 먼저 복사합니다.
    std::vector<uint32_t> currentProjectileIds;
    currentProjectileIds.reserve(m_projectiles.size());

    for (const auto& [id, projectile] : m_projectiles)
    {
        
        currentProjectileIds.push_back(id);
    }

    // 복사해 둔 ID로 안전하게 순회합니다.
    for (uint32_t id : currentProjectileIds)
    {
        // 순회 도중 다른 투사체에 의해 이미 지워졌을 수 있으므로 존재 여부를 확인합니다.
        auto it = m_projectiles.find(id);
        if (it == m_projectiles.end())
        {
            continue;
        }

        auto& projectile = it->second;

        if (!projectile || !projectile->IsActive())
        {
            m_projectiles.erase(it);
            continue;
        }

        auto enemies = m_enemyManager.FindEnemyInRange(
            projectile->GetPosition(),
            0.1f,
            -1
        );

        projectile->Update(deltaTime);
    }
}