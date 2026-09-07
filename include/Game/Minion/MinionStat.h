#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>
#include <functional>

#include "Game/Skill/BuffSkill.h"
#include "Game/Skill/SkillData.h"

class MinionData;
//class SkillData;

struct MinionBaseStat {
    int power = 0;
    float attackCooldown = 0.0f;
    float attackRange = 0.0f;
    int skillMaxTargets = 1;
    int maxAC = 0;

    DefaultAttackSpec attackSpec;
};

struct MinionCurrentStat {
    int power = 0;
    float attackCooldown = 0.0f;
    float attackRange = 0.0f;
    int skillMaxTargets = 1;
    int maxAC = 0;
    int currentAC = 0;

    DefaultAttackSpec attackSpec;
};

class MinionStat
{
public:
    MinionStat();

    void Init(const MinionData& data, const SkillData& skillData);
    void Recalculate();
    void Update(float deltaTime);

    // 기존 베이스/스탯 조작 함수
    void AddBasePower(int value) { m_base.power += value; Recalculate(); }
    void AddBaseAttackCooldown(float value) { m_base.attackCooldown = std::max(0.1f, m_base.attackCooldown - value); Recalculate(); }
    void AddBaseMaxAC(int value) { m_base.maxAC = std::max(1, m_base.maxAC - value); Recalculate(); }
    void AddBaseSkillMaxTargets(int value) { m_base.skillMaxTargets += value; Recalculate(); }
    void AddBaseRange(float value) { m_base.attackRange += value; Recalculate(); }

    // 버프 추가 함수
    void AddPowerBuff(const BuffData& data)
    {
        m_powerBuffs.push_back(data);
    }

    void AddAttackSpeedBuff(const BuffData& data)
    {
        m_attackSpeedBuffs.push_back(data);
    }

    void AddMaxTargetsBuff(BuffData& data)
    {
        m_maxTargetsBuffs.push_back(data);
    }

    void AddAttackSpecBuff(const DefaultAttackSpec& newSpec, float duration)
    {
        m_attackSpecBuffs.push_back({ newSpec, duration });
    }

    void ClearBuffs();

    void AddAC(int value) { m_current.currentAC += value; }
    void ResetCurrentAC() { m_current.currentAC = 0; }

    const bool& CheckChangedDefaultAttack()
    {
        return m_hasCustomAttackSpec;
    }

    const MinionBaseStat& GetBase() const { return m_base; }
    const MinionCurrentStat& GetCurrent() const { return m_current; }

private:
    MinionBaseStat m_base;
    MinionCurrentStat m_current;

    bool m_hasCustomAttackSpec = false;

    // 적용 중인 버프
    std::vector<BuffData> m_powerBuffs;
    std::vector<BuffData> m_attackSpeedBuffs;
    std::vector<BuffData> m_maxTargetsBuffs;
    std::vector<AttackSpecBuffContext> m_attackSpecBuffs;
};