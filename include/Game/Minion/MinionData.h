#pragma once

#include <cstdint>
#include <string>

#include "Game/Skill/SkillTypes.h"
#include "Game/Minion/StatUpgrade.h"

struct MinionData
{
    uint32_t id;

    int hp;
    int attackPower;
    int maxAC; 
    float attackCooldown;
    float attackRange;

    std::unordered_map<int, UpgradeSlot> upgradeSlots;
    
    int skillId;
    DeliveryType deliveryType;
    SearchScope searchScope;
    int maxTargets;

    ProjectileType projectileType = ProjectileType::None;
    ProjectileHitType projectileHitType = ProjectileHitType::None;
    float projectileSpeed = 0.0f;
    float projectileDuration = 0.0f;

    std::string minionType;
    int cost;
};