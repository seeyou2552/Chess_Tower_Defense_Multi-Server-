#include "Game/Skill/HitEvent.h"

#include "Game/Minion/Minion.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Room.h"

void SlowOnHitEvent::OnHit(
    const std::shared_ptr<Minion>&,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>&
)
{
    if (!enemy)
        return;

    enemy->ApplySlow(data.value, data.duration);
}

void StunOnHitEvent::OnHit(
    const std::shared_ptr<Minion>&,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>&
)
{
    if (!enemy)
        return;

    enemy->ApplyStun(data.duration);
}

void DamageOverTimeOnHitEvent::OnHit(
    const std::shared_ptr<Minion>&,
    const std::shared_ptr<Enemy>& enemy,
    const HitEventData& data,
    const std::shared_ptr<Room>&
)
{
    if (!enemy)
        return;

    enemy->ApplyDamageOverTime(data.value, data.duration);
}
