#include "Game/Skill/SkillStrategy.h"
#include "Game/Skill/SkillTypes.h"
#include "Game/Skill/SkillManager.h"
#include "Game/Skill/SkillFactory.h"
#include "Game/Skill/HitEvent.h"
#include "Game/Skill/KillEvent.h"


#include "Core/Logger.h"


// Enemy Target
std::shared_ptr<IEnemyTargetSkillStrategy> SkillFactory::EnemyTargetCreate(SkillType type)
{
    switch (type)
    {
    case SkillType::Impact:
        return std::make_shared<ImpactSkillStrategy>();
    case SkillType::Projectile:
        return std::make_shared<ProjectileSkillStrategy>();
    case SkillType::AOE:
        return std::make_shared<AOESkillStrategy>();
    default:
        return nullptr;
    }
}

// Minion Target
std::shared_ptr<IMinionTargetSkillStrategy> SkillFactory::MinionTargetCreate(SkillType type)
{
    switch (type)
    {
    case SkillType::Buff:
        return std::make_shared<BuffSkillStrategy>();
    default:
        return nullptr;
    }
}

// Buff

std::shared_ptr<IBuffSkillStrategy> SkillFactory::BuffSkillCreate(int id)
{
    switch (id)
    {
        case 4001:
            return std::make_shared<AtkSpeedBuff>();
        default:
            return nullptr;
    }
}

// HIt & Kill

std::shared_ptr<IKillEvent> SkillFactory::KillEventCreate(int id)
{
    switch (id)
    {
    case 3001:
        return std::make_shared<GoldOnKillEvent>();
    case 3002:
        return std::make_shared<PowerUpOnKillEvent>();
    default:
        return nullptr;
    }
}

std::shared_ptr<IHitEvent> SkillFactory::HitEventCreate(int id)
{
    switch (id)
    {
        case 2004:
            return std::make_shared<SlowOnHitEvent>();
        case 2005:
            return std::make_shared<StunOnHitEvent>();
        case 2001:
            return std::make_shared<DamageOverTimeOnHitEvent>();
        case 2002:
            return std::make_shared<ExcutionOnHitEvent>();
        case 2003:
            return std::make_shared<AOEOnHitEvent>();
        default:
            return nullptr;
    }
}