#include "Game/Enemy/Enemy.h"
#include "Game/Minion/Minion.h"
#include "Game/Math/Vector3.h"

#include "Core/Logger.h"

#include <functional>

#include <cmath>

Enemy::Enemy()
{

}

void Enemy::Init(
    uint32_t instanceId,
    const EnemyData& data,
    int x,
    int y
)
{
    m_instanceId = instanceId;
    m_dataId = data.id;

    m_hp = data.maxHp;
    m_power = data.attack;
    m_moveSpeed = data.moveSpeed;

    m_position = { float(x), float(y), 0 };

    m_reward = data.reward;

    m_isDead = false;
    m_active = true;
}

void Enemy::Reset()
{
    m_instanceId = 0;

    m_dataId = 0;

    m_position = { 0,0,0 };

    m_active = false;

    m_hp = 0;
    m_power = 0;
    m_moveSpeed = 0;
    m_slowPercent = 0.0f;
    m_stunDuration = 0.0f;
    m_isStunned = false;
    m_isDead = false;
    m_lifeTime = 0.0f;
    m_slowDebuffs.clear();
    m_damageOverTimeDebuffs.clear();
}

void Enemy::Update(
    float deltaTime
)
{
    if (m_isDead)
        return;

    m_lifeTime += deltaTime;
    UpdateDebuffs(deltaTime);

    if (!m_isStunned)
    {
        UpdateMovement(deltaTime);
    }
}

void Enemy::SetPath(
    int pathIndex,
    const std::vector<Vector3>& waypoints
)
{
    m_pathIndex = pathIndex;
    m_currentWaypointIndex = 1;
    m_waypoints = waypoints;
}

void Enemy::UpdateMovement(float deltaTime)
{
    if (m_isStunned)
        return;

    // 더 이상 진행 할 수 없을 시 목적지 도달 로직 실행
    if (m_waypoints.empty() || m_currentWaypointIndex >= m_waypoints.size())
    {
        OnReachEnd();
        return;
    }

    const Vector3& target = m_waypoints[m_currentWaypointIndex];

    float dx = target.x - m_position.x;

    float dy = target.y - m_position.y;

    float distance =
        std::sqrt(
            dx * dx +
            dy * dy
        );

    if (distance <= 0.001f)
    {
        ++m_currentWaypointIndex;

        return;
    }

    const float moveMultiplier = GetMovementSpeedMultiplier();
    float moveDistance =
        m_moveSpeed * moveMultiplier * deltaTime;

    if (moveDistance >= distance)
    {
        m_position = target;

        ++m_currentWaypointIndex;

        return;
    }

    float dirX = dx / distance;
    float dirY = dy / distance;

    m_position.x += dirX * moveDistance;

    m_position.y += dirY * moveDistance;
}

void Enemy::ApplySlow(float percent, float duration)
{
    if (percent <= 0.0f || duration <= 0.0f)
        return;

    m_slowDebuffs.push_back({ percent, duration });
    RecalculateSlowPercent();
}

void Enemy::ApplyStun(float duration)
{
    if (duration <= 0.0f)
        return;

    m_isStunned = true;
    m_stunDuration = std::max(m_stunDuration, duration);
}

void Enemy::ApplyDamageOverTime(float damage, float duration)
{
    if (damage <= 0.0f || duration <= 0.0f)
        return;

    m_damageOverTimeDebuffs.push_back({ damage, duration, 1.0f, 0.0f });
}

void Enemy::UpdateDebuffs(float deltaTime)
{
    // Slow
    for (auto it = m_slowDebuffs.begin(); it != m_slowDebuffs.end();)
    {
        it->remainingTime -= deltaTime;

        if (it->remainingTime <= 0.0f)
            it = m_slowDebuffs.erase(it);
        else
            ++it;
    }

    RecalculateSlowPercent();

    // Stun
    if (m_isStunned)
    {
        m_stunDuration -= deltaTime;
        if (m_stunDuration <= 0.0f)
        {
            m_isStunned = false;
            m_stunDuration = 0.0f;
        }
    }

    // Damage
    for (auto it = m_damageOverTimeDebuffs.begin(); it != m_damageOverTimeDebuffs.end();)
    {
        it->remainingTime -= deltaTime;
        it->elapsedTime += deltaTime;

        while (it->elapsedTime >= it->tickInterval && !m_isDead)
        {
            TakeDamage(static_cast<int>(it->damagePerTick));
            it->elapsedTime -= it->tickInterval;
        }

        if (it->remainingTime <= 0.0f || m_isDead)
            it = m_damageOverTimeDebuffs.erase(it);
        else
            ++it;
    }
}

void Enemy::RecalculateSlowPercent()
{
    float multiplier = 1.0f;

    for (const auto& slow : m_slowDebuffs)
    {
        const float ratio = std::max(0.0f, 1.0f - (slow.percent / 100.0f));
        multiplier *= ratio;
    }

    m_slowPercent = std::clamp((1.0f - multiplier) * 100.0f, 0.0f, 100.0f);
}

void Enemy::OnReachEnd()
{
    if (m_onArrival)
    {
        auto selfEnemy = std::static_pointer_cast<Enemy>(shared_from_this());
        m_onArrival(selfEnemy);
    }
}

void Enemy::TakeDamage(int damage)
{
    m_hp -= damage;
    if (m_hp <= 0)
    {
        
        m_isDead = true;
    }
}