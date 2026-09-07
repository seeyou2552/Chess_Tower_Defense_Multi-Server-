#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "Game/Skill/SkillTypes.h"

class Minion;

struct DefaultAttackSpec
{
    DeliveryType deliveryType;
    SearchScope searchScope;
    int maxTargets;

    ProjectileType projectileType;
    ProjectileHitType projectileHitType;
    float projectileSpeed;
    float projectileDuration;
};

struct BuffData {
    int id;
    float value;
    float duration;
    std::optional<DefaultAttackSpec> changeAttackSpec;
};

struct AttackSpecBuffContext
{
    DefaultAttackSpec attackSpec;
    float duration;
};

class IBuffSkillStrategy {
public:
    virtual ~IBuffSkillStrategy() = default;
    virtual void OnBuff(std::vector<std::shared_ptr<Minion>>& minions, const BuffData& data) const = 0;
};

class PowerUpBuff final : public IBuffSkillStrategy {
public:
    void OnBuff(std::vector<std::shared_ptr<Minion>>& minions, const BuffData& data) const override;
};

class AtkSpeedBuff final : public IBuffSkillStrategy {
public:
    void OnBuff(std::vector<std::shared_ptr<Minion>>& minions, const BuffData& data) const override;
};