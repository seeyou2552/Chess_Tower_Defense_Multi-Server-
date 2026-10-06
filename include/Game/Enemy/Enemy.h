#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <mutex>
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

    uint32_t GetInstanceId() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_instanceId;
    }

    uint32_t GetId() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_dataId;
    }

    int GetMaxHp() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_maxHp;
    }

    int GetHp() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_hp;
    }

    void SetPath(
        int pathIndex,
        const std::vector<Vector3>& waypoints
    );

    int GetPathIndex() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_pathIndex;
    }

    Vector3 GetPosition() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_position;
    }

    void UpdateMovement(float deltaTime);

    void OnReachEnd();

    void SetArrivalCallback(ArrivalCallback callback)
    { 
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_onArrival = callback; 
    }

    void SetDeadCallback(DeadCallback callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_onDead = callback;
    }

    int GetDamage() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_power;
    }

    int GetReward() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_reward;
    }

    bool IsActive() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_active;
    }

    void TakeDamage(int damage);

    bool IsDead() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_isDead;
    }

    void ApplySlow(float percent, float duration);
    void ApplyStun(float duration);
    void ApplyDamageOverTime(float damage, float duration);

    bool IsStunned() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_isStunned;
    }

    float GetSlowPercent() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return m_slowPercent;
    }

    float GetMovementSpeedMultiplier() const
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return std::max(0.0f, 1.0f - (m_slowPercent / 100.0f));
    }

private:
    void UpdateDebuffs(
        float deltaTime,
        DeadCallback& deadCallback,
        std::shared_ptr<Enemy>& selfEnemy
    );
    void UpdateMovementLocked(
        float deltaTime,
        ArrivalCallback& arrivalCallback,
        std::shared_ptr<Enemy>& selfEnemy
    );
    void TakeDamageLocked(
        int damage,
        DeadCallback& deadCallback,
        std::shared_ptr<Enemy>& selfEnemy
    );
    void RecalculateSlowPercent();

private:

    mutable std::recursive_mutex m_mutex;

    uint32_t m_instanceId = 0;
    uint32_t m_dataId = 0;

    int m_pathIndex = 0;
    int m_currentWaypointIndex = 0;

    int m_maxHp = 0;
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