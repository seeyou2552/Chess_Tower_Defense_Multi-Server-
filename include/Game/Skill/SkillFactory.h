#pragma once

#include <memory>

#include "Game/Skill/BuffSkill.h"
#include "Game/Skill/SkillStrategy.h"
#include "Game/Skill/HitEvent.h"
#include "Game/Skill/KillEvent.h"

class SkillFactory
{
public:

    static std::shared_ptr<IEnemyTargetSkillStrategy> EnemyTargetCreate(SkillType type);
    static std::shared_ptr<IMinionTargetSkillStrategy> MinionTargetCreate(SkillType type);

    static std::shared_ptr<IBuffSkillStrategy> BuffSkillCreate(int id);

    static std::shared_ptr<IKillEvent> KillEventCreate(int id);
    static std::shared_ptr<IHitEvent> HitEventCreate(int id);
};