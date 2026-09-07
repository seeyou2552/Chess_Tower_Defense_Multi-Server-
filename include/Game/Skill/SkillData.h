#pragma once

#include <string>
#include <vector>

#include "Game/Skill/SkillTypes.h"
#include "Game/Skill/BuffSkill.h"
#include "Game/Skill/KillEvent.h"
#include "Game/Skill/HitEvent.h"

struct SkillData {
    unsigned int id;
    SkillType skillType;         
    SearchScope searchScope;       
    int maxTargets;
    ProjectileType projectileType;    
    ProjectileHitType projectileHitType; 
    float projectileSpeed;
    float projectileDuration;

    int skillDamage;
    float skillScale;

    std::vector<BuffData> buffList;
    std::vector<HitEventData> hitEvents;
    std::vector<KillEventData> killEvents;

};