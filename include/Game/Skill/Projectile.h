#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Game/Math/Vector3.h"
#include "Game/Skill/SkillTypes.h"

class Enemy;
class Minion;
class EnemyManager;

class Projectile
{
public:
    using AttackCallback = std::function<void(
        std::shared_ptr<Enemy>&,
        std::shared_ptr<Minion>&,
        int
        )>;

    using ReturnCallback = std::function<void(uint32_t)>;

public:
    Projectile();

    void Init(
        uint32_t instanceId,
        std::shared_ptr<Enemy>& target,
        Vector3 startPos,
        std::shared_ptr<Minion>& minion,
        int damage,
        float speed,
        float activeDuration,
        bool isChase,
        ProjectileHitType hitType,
        EnemyManager& enemyManager
    );

    void Reset();

    void Update(float deltaTime);

    bool IsActive() const
    {
        return m_active;
    }

    uint32_t GetInstanceId() const
    {
        return m_instanceId;
    }

    const Vector3& GetPosition() const
    {
        return m_position;
    }

    void SetAttackCallback(AttackCallback callback)
    {
        m_onAttack = std::move(callback);
    }

    void SetReturnCallback(ReturnCallback callback)
    {
        m_onReturn = std::move(callback);
    }

	const ProjectileHitType& GetHitType() const
	{
		return m_hitType;
	}

private:
    void Movement(float deltaTime);

    void Chase(float deltaTime);
    void Direct(float deltaTime);

    void CheckCollision(
        const Vector3& previousPosition,
        const Vector3& currentPosition
    );

    bool CheckEnemyCollision(
        const Vector3& previousPosition,
        const Vector3& currentPosition,
        const std::shared_ptr<Enemy>& enemy
    ) const;

    void OnHit(
        std::shared_ptr<Enemy>& enemy
    );

    void ReturnToPool();

private:
    uint32_t m_instanceId = 0;

    Vector3 m_position{};
    Vector3 m_direction{};
    Vector3 m_previousPosition{};

    int m_damage = 0;

    float m_speed = 0.0f;
    float m_activeDuration = 0.0f;
    float m_elapsedTime = 0.0f;

    bool m_isChase = false;
    bool m_active = false;

    std::shared_ptr<Enemy> m_target;
    std::shared_ptr<Minion> m_ownerMinion;

    ProjectileHitType m_hitType = ProjectileHitType::Impact;

    // 실제로 Hit가 발생한 Enemy만 저장
    std::unordered_map<
        uint32_t,
        std::shared_ptr<Enemy>
    > m_hitedEnemies;

    //Projectile Collider의 반지름
    static constexpr float ProjectileRadius = 0.1f;

    // Enemy Collider의 반지름.
    static constexpr float EnemyRadius = 0.5f;

    EnemyManager* m_enemyManager = nullptr;

private:
    AttackCallback m_onAttack;
    ReturnCallback m_onReturn;
};