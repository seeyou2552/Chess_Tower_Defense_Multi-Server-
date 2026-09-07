#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Game/Enemy/Enemy.h"

enum class DeliveryType : uint8_t
{
    Instant,
    Projectile
};

enum class SkillType : uint8_t
{
    Impact = 0 ,
    Projectile = 1,
    AOE = 2,
    Buff = 3
};

enum class SearchScope : uint8_t
{
    SingleTarget = 0,
    MultipleTarget = 1,
    Area = 2
};

enum class ProjectileType : uint8_t
{
    None = 2,
    Straight = 1,
    Chase = 0 
};

enum class ProjectileHitType : uint8_t
{
    None = 2,
    Impact = 0,
    Pierce = 1
};

enum class TargetType : uint8_t
{
    Enemy = 0,
    Minion = 1
};

enum class AttackType : uint8_t
{
	Default = 0,
	Skill = 1
};

struct AttackEvent
{
    AttackType attackType;
    bool isProjectile;

    uint32_t minionInstanceId;
    std::vector<uint32_t> targets;
    int damage;
};