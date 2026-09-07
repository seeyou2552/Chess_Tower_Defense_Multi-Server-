#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

#include "Game/Enemy/EnemyData.h"
#include "Game/Math/Vector3.h"

class Minion;

struct SlowDebuffData
{
    float percent = 0.0f;
    float remainingTime = 0.0f;
};

struct DamageOverTimeDebuffData
{
    float damagePerTick = 0.0f;
    float remainingTime = 0.0f;
    float tickInterval = 1.0f;
    float elapsedTime = 0.0f;
};

class Enemy : public std::enable_shared_from_this<Enemy>
{
public:

    explicit Enemy();

    using ArrivalCallback = std::function<void(std::shared_ptr<Enemy>)>;
    using DeadCallback = std::function<void(std::shared_ptr<Enemy>)>;

    void Init(
        uint32_t instanceId,
        const EnemyData& data,
        int x,
        int y
    );

    void Reset();

    void Update(float deltaTime);

    const uint32_t& GetInstanceId() const
    {
        return m_instanceId;
    }

    uint32_t GetId() const
    {
        return m_dataId;
    }

    int GetHp() const
    {
        return m_hp;
    }

    void SetPath(
        int pathIndex,
        const std::vector<Vector3>& waypoints
    );

    int GetPathIndex() const
    {
        return m_pathIndex;
    }

    const Vector3& GetPosition() const
    {
        return m_position;
    }

    void UpdateMovement(float deltaTime);

    void OnReachEnd();

    void SetArrivalCallback(ArrivalCallback callback)
    { 
        m_onArrival = callback; 
    }

    int GetDamage() const
    {
        return m_power;
    }

    int GetReward() const
    {
        return m_reward;
    }

    bool IsActive()
    {
        return m_active;
    }

    void TakeDamage(int damage);

    bool IsDead()
    {
        return m_isDead;
    }

    void ApplySlow(float percent, float duration);
    void ApplyStun(float duration);
    void ApplyDamageOverTime(float damage, float duration);

    bool IsStunned() const
    {
        return m_isStunned;
    }

    float GetSlowPercent() const
    {
        return m_slowPercent;
    }

    float GetMovementSpeedMultiplier() const
    {
        return std::max(0.0f, 1.0f - (m_slowPercent / 100.0f));
    }

private:
    void UpdateDebuffs(float deltaTime);
    void RecalculateSlowPercent();

private:

    uint32_t m_instanceId = 0;
    uint32_t m_dataId = 0;

    int m_pathIndex = 0;
    int m_currentWaypointIndex = 0;

    int m_hp = 0;
    int m_power = 0;
    float m_moveSpeed = 0;
    float m_slowPercent = 0.0f;
    float m_stunDuration = 0.0f;
    bool m_isStunned = false;

    Vector3 m_position{};

    int m_reward = 0;

    float m_lifeTime = 0.0f;

    bool m_isDead = false;
    bool m_active = false;

    std::vector<SlowDebuffData> m_slowDebuffs;
    std::vector<DamageOverTimeDebuffData> m_damageOverTimeDebuffs;

    std::vector<Vector3> m_waypoints;    

    ArrivalCallback m_onArrival;
    DeadCallback m_onDead;
};