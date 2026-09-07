#include "Game/Minion/MinionStat.h"
#include "Game/Minion/MinionData.h"
#include "Game/Skill/BuffSkill.h"
#include "Game/Skill/SkillData.h"

#include "Core/Logger.h"

MinionStat::MinionStat()
{

}

void MinionStat::Init(const MinionData& data, const SkillData& skillData)
{
    m_base.power = data.attackPower;
    m_base.attackCooldown = data.attackCooldown;
    m_base.attackRange = data.attackRange / 2.0f;
    m_base.maxAC = data.maxAC;
    m_base.skillMaxTargets = skillData.searchScope == SearchScope::SingleTarget
        ? 1
        : skillData.maxTargets;
    
    // 기본공격 관련 
    m_base.attackSpec.deliveryType = data.deliveryType;
    m_base.attackSpec.searchScope = data.searchScope;
    m_base.attackSpec.maxTargets = data.searchScope == SearchScope::SingleTarget
        ? 1
        : data.maxTargets;

    m_base.attackSpec.projectileType = data.projectileType;
    m_base.attackSpec.projectileHitType = data.projectileHitType;
    m_base.attackSpec.projectileSpeed = data.projectileSpeed;
    m_base.attackSpec.projectileDuration = data.projectileDuration;

    ClearBuffs();
    Recalculate();
}

void MinionStat::Recalculate()
{
    float powerBonus = 0.0f;
    for (const auto& buff : m_powerBuffs)
        powerBonus += buff.value;

    float attackSpeedBonus = 0.0f;
    for (const auto& buff : m_attackSpeedBuffs)
        attackSpeedBonus += buff.value;

    m_current.power = static_cast<int>(m_base.power * (1.0f + powerBonus / 100.0f));
    m_current.attackCooldown = std::max(
        0.1f,
        m_base.attackCooldown / (1.0f + attackSpeedBonus / 100.0f)
    );

    if (!m_attackSpecBuffs.empty())
    {
        const auto& latestBuff = m_attackSpecBuffs.back();

        m_current.attackSpec = latestBuff.attackSpec; // 최신 버프 스펙 적용
        m_hasCustomAttackSpec = true;
    }
    else
    {
        // 모든 버프가 끝나면 기본 스펙으로 복원
        m_current.attackSpec = m_base.attackSpec;
        m_hasCustomAttackSpec = false;
    }

    m_current.attackRange = m_base.attackRange;
    m_current.maxAC = m_base.maxAC;
    m_current.skillMaxTargets = m_base.skillMaxTargets;
}

void MinionStat::ClearBuffs()
{
    m_powerBuffs.clear();
    m_attackSpeedBuffs.clear();
    m_maxTargetsBuffs.clear();
    m_attackSpecBuffs.clear();
}

void MinionStat::Update(float deltaTime)
{
    // 걸려있는 버프가 하나도 없다면 타이머 연산을 아예 생략
    if (m_powerBuffs.empty() && m_attackSpeedBuffs.empty() && m_maxTargetsBuffs.empty() && m_attackSpecBuffs.empty())
        return;

    bool isChanged = false;

    // 람다를 이용해 지속 시간이 0 이하가 된 버프들을 일괄 처리
    auto updateBuffList = [deltaTime, &isChanged](std::vector<BuffData>& buffs) {
        auto it = std::remove_if(buffs.begin(), buffs.end(), [deltaTime, &isChanged](BuffData& buff) {
            // 지속 시간이 영구적인(-1 등) 것이 아니라면 시간을 감소시킴
            if (buff.duration > 0.0f)
            {
                buff.duration -= deltaTime;
                if (buff.duration <= 0.0f)
                {
                    isChanged = true;
                    return true; // 삭제 대상 판정
                }
            }
            return false;
        });

        if (it != buffs.end())
        {
            buffs.erase(it, buffs.end());
        }
        };

    updateBuffList(m_powerBuffs);
    updateBuffList(m_attackSpeedBuffs);

    // 공격 스펙 버프 시간 감소 및 만료 처리
    auto it = std::remove_if(m_attackSpecBuffs.begin(), m_attackSpecBuffs.end(), [deltaTime, &isChanged](AttackSpecBuffContext& buff) {
        if (buff.duration > 0.0f)
        {
            buff.duration -= deltaTime;
            if (buff.duration <= 0.0f)
            {
                isChanged = true;
                return true; // 시간이 다 되면 삭제 대상
            }
        }
        return false;
        });

    if (it != m_attackSpecBuffs.end())
    {
        m_attackSpecBuffs.erase(it, m_attackSpecBuffs.end());
    }

    // 시간이 다 되어 삭제된 버프가 있다면 스탯을 재계산
    if (isChanged)
    {
        Recalculate();
    }
}