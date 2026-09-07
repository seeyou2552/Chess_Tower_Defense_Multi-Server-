#include "Game/Skill/SkillStrategy.h"
#include "Game/Skill/SkillFactory.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Minion/Minion.h"
#include "Game/Room.h"
#include "Game/Skill/SkillManager.h"
#include "Game/Skill/SkillData.h"

#include "Network/Packet/MinionPacket.h"
#include "Network/Packet/PacketType.h"
#include "Network/Packet/PacketWriter.h"

#include "Core/Logger.h"

SkillExecution CreateExecution(
    SkillType type,
    DeliveryType deliveryType,
    const SkillData& data
)
{
    return {
        type,
        data.skillDamage,
        data.skillScale,
        deliveryType,
        data.projectileType,
        data.projectileHitType,
        data.projectileSpeed,
        data.projectileDuration
    };
}

// Enemy Target

void ImpactSkillStrategy::Execute(
    std::shared_ptr<Minion> minion,
    std::vector<std::shared_ptr<Enemy>>& targets,
    const SkillData& data,
    std::shared_ptr<Room> room
) const
{
    auto damage = minion->GetPower() + data.skillDamage;
    room->DamageToEnemy(
        targets,
        minion,
        damage
    );
}

void ProjectileSkillStrategy::Execute(
    std::shared_ptr<Minion> minion,
    std::vector<std::shared_ptr<Enemy>>& targets,
    const SkillData& data,
    std::shared_ptr<Room> room
) const
{
   
    std::vector<std::shared_ptr<Projectile>> projectiles;

    for (auto& enemy : targets)
    {
        room->SpawnProjectile(
            minion,
            enemy,
            data.projectileHitType
        );
    }
}

void AOESkillStrategy::Execute(
    std::shared_ptr<Minion> minion,
    std::vector<std::shared_ptr<Enemy>>& targets,
    const SkillData& data,
    std::shared_ptr<Room> room
) const
{
    Logger::GetInstance().Info("AOE Start");

    auto damage = minion->GetPower() + data.skillDamage;
    room->DamageToEnemy(
        targets,
        minion,
        damage
    );
}

// Minion Target

void BuffSkillStrategy::Execute(
    std::shared_ptr<Minion> minion,
    std::vector<std::shared_ptr<Minion>>& targets,
    const SkillData& data,
    std::shared_ptr<Room> room
) const
{
    for (auto& buff : minion->GetBuffData())
    {
        SkillManager::GetInstance().ExecuteBuffSkill(buff.id, targets, buff);
    }

    // 버프 최종 적용 후 스텟계산 로직 실행
    for (auto& target : targets)
    {
        target->Recalculate();
    }
}
