#include "Game/Skill/HitEvent.h"

#include "Game/Minion/Minion.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Math/Vector3.h"
#include "Game/Room.h"

#include "Core/Logger.h"

void SlowOnHitEvent::OnHit(
    const std::shared_ptr<Minion>& minion,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>& room
)
{
    if (!enemy)
        return;

    enemy->ApplySlow(data.value, data.duration);
}

void StunOnHitEvent::OnHit(
    const std::shared_ptr<Minion>& minion,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>& room
)
{
    if (!enemy)
        return;

    enemy->ApplyStun(data.duration);
}

void DamageOverTimeOnHitEvent::OnHit(
    const std::shared_ptr<Minion>& minion,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>& room
)
{
    if (!enemy)
        return;

    enemy->ApplyDamageOverTime(data.value, data.duration);
}

void AOEOnHitEvent::OnHit(
    const std::shared_ptr<Minion>& minion,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>& room
)
{
    if (!enemy)
        return;

    auto& enemyManager = room->GetEnemyManager();

    const Vector3 enemyPos = enemy->GetPosition();
    auto range = data.value * 2.0f;

    auto targets = enemyManager.FindEnemyInRange(
        enemyPos,
        range,
        0
    );

    for (auto& enemy : targets)
    {
        enemy->TakeDamage(data.value);
    }
    
}

void ExcutionOnHitEvent::OnHit(
    const std::shared_ptr<Minion>& minion,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>& room
)
{
    if (!enemy)
        return;
    
    int maxHp = enemy->GetMaxHp();

    // execution 의 percentage(value)를 넘으면 최대 체력만큼 데미지 
    if (enemy->GetHp() <= maxHp * float(data.value / 100.0f))
    {
        enemy->TakeDamage(maxHp);
    }
}
