#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "Game/Skill/SkillTypes.h"

class SkillData;
class Enemy;
class Minion;
class Room;

struct SkillExecution
{
    SkillType type;
    int damage;
    float scale;
    DeliveryType deliveryType;
    ProjectileType projectileType;
    ProjectileHitType projectileHitType;
    float projectileSpeed;
    float projectileDuration;
};

using SkillExecutionCallback = std::function<void(
    std::shared_ptr<Minion>,
    std::vector<std::shared_ptr<Enemy>>,
    const SkillExecution&
)>;

// Enemy Target

class IEnemyTargetSkillStrategy
{
public:
    virtual ~IEnemyTargetSkillStrategy() = default;

    virtual void Execute(
        std::shared_ptr<Minion> minion,
        std::vector<std::shared_ptr<Enemy>>& targets,
        const SkillData& data,
        std::shared_ptr<Room> room
    ) const = 0;
};

class ImpactSkillStrategy final : public IEnemyTargetSkillStrategy
{
public:
    void Execute(
        std::shared_ptr<Minion> minion,
        std::vector<std::shared_ptr<Enemy>>& targets,
        const SkillData& data,
        std::shared_ptr<Room> room
    ) const override;
};

class ProjectileSkillStrategy final : public IEnemyTargetSkillStrategy
{
public:
    void Execute(
        std::shared_ptr<Minion> minion,
        std::vector<std::shared_ptr<Enemy>>& targets,
        const SkillData& data,
        std::shared_ptr<Room> room
    ) const override;
};

class AOESkillStrategy final : public IEnemyTargetSkillStrategy
{
public:
    void Execute(
        std::shared_ptr<Minion> minion,
        std::vector<std::shared_ptr<Enemy>>& targets,
        const SkillData& data,
        std::shared_ptr<Room> room
    ) const override;
};

// Minion Target
class IMinionTargetSkillStrategy
{
public:
    virtual ~IMinionTargetSkillStrategy() = default;

    virtual void Execute(
        std::shared_ptr<Minion> minion,
        std::vector<std::shared_ptr<Minion>>& targets,
        const SkillData& data,
        std::shared_ptr<Room> room
    ) const = 0;
};

class BuffSkillStrategy final : public IMinionTargetSkillStrategy
{
public:
    void Execute(
        std::shared_ptr<Minion> minion,
        std::vector<std::shared_ptr<Minion>>& targets,
        const SkillData& data,
        std::shared_ptr<Room> room
    ) const override;
};