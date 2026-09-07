#include "Game/Minion/Minion.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Skill/SkillDataManager.h"
#include "Game/Skill/SkillFactory.h"
#include "Game/Minion/StatUpgrade.h"
#include "Game/Minion/MinionStat.h"
#include "Game/Minion/MinionData.h"

#include <memory>

#include "Core/Logger.h"

Minion::Minion()
    :
    m_stat()
{

}

void Minion::Init(
    uint32_t instanceId,
    uint32_t orderId,
    const MinionData& data,
    int x,
    int y
)
{
    m_atkTimer = 0.0f;
    m_instanceId = instanceId;
    m_orderId = orderId;
    m_dataId =
        data.id;

    m_position = { float(x) / 2.0f, float(y) / 2.0f, 0.0f };

    m_upgradeSlots = data.upgradeSlots;

    m_skillData = SkillDataManager::GetInstance().GetData(data.skillId);

    m_enemyTargetSkillStrategy = m_skillData
        ? SkillFactory::EnemyTargetCreate(m_skillData->skillType)
        : nullptr;
    m_minionTargetSkillStrategy = m_skillData
        ? SkillFactory::MinionTargetCreate(m_skillData->skillType)
        : nullptr;

    m_stat.Init(data, *m_skillData);
    m_stat.ResetCurrentAC();


    // KillEvent
    for (auto& killEvent : m_skillData->killEvents)
    {
        auto event = SkillFactory::KillEventCreate(killEvent.id);

        if (event)
        {
            m_killEvents.push_back(event);
        }
    }

    // Hit Event
    for (auto& hitEvent : m_skillData->hitEvents)
    {
        auto event = SkillFactory::HitEventCreate(hitEvent.id);

        if (event)
        {
            m_hitEvents.push_back(event);
        }
    }

    m_sellPrice = data.cost;
    m_active = true;
}

void Minion::Reset()
{
    m_atkTimer = 0.0f;
    m_instanceId = 0;

    m_dataId = 0;

    m_active = false;
    
    m_onDefaultAttack = nullptr;
    m_onEnemyTargetSkill = nullptr;
    m_onMinionTargetSkill = nullptr;

    m_skillData = nullptr;
    m_enemyTargetSkillStrategy = nullptr;
    m_minionTargetSkillStrategy = nullptr;

    m_killEvents.clear();
    m_hitEvents.clear();
}

void Minion::Update(float deltaTime)
{
    m_stat.Update(deltaTime);
    m_atkTimer += deltaTime;    

    if (m_atkTimer < m_stat.GetCurrent().attackCooldown)
        return;

    m_attackIsReady = true;

    if (m_stat.GetCurrent().currentAC >= m_stat.GetCurrent().maxAC)
    {
        m_skillIsReady = true;
    }
}

const int& Minion::GetMaxTargets() const
{
    if (m_stat.GetCurrent().currentAC >= m_stat.GetCurrent().maxAC)
    {
        switch (m_skillData->searchScope)
        {
            case SearchScope::SingleTarget:
                return 1;

            case SearchScope::MultipleTarget:
                Logger::GetInstance().Info("MaxTarget = " + std::to_string(m_stat.GetCurrent().skillMaxTargets));
				return m_stat.GetCurrent().skillMaxTargets;

            case SearchScope::Area:
                return -1;
        }

    }

    return m_stat.GetCurrent().attackSpec.maxTargets;
    
} 

void Minion::OnEnemyTarget(std::vector<std::shared_ptr<Enemy>>& targets)
{
    // Skill
    if (m_skillIsReady)
    {
        m_stat.ResetCurrentAC();
        m_skillIsReady = false;

        if (m_onEnemyTargetSkill)
        {
            m_onEnemyTargetSkill(m_instanceId, targets);
            m_skillIsReady = false;
            Logger::GetInstance().Info("Skill Event~~~~~~~~~");
        }
    }

    // 기본 공격
    else 
    {
        m_stat.AddAC(1);
        if (m_onDefaultAttack)
        {
            m_onDefaultAttack(m_instanceId, targets);
        }
    }

    m_atkTimer = 0.0f;
    m_attackIsReady = false;
}

void Minion::OnMinionTarget(std::vector<std::shared_ptr<Minion>> targets)
{
    // Skill
    if (m_skillIsReady)
    {
        m_stat.ResetCurrentAC();
        m_skillIsReady = false;

        if (m_onMinionTargetSkill)
        {
            m_onMinionTargetSkill(m_instanceId, targets);
            m_skillIsReady = false;
        }
    }

    m_atkTimer = 0.0f;
    m_attackIsReady = false;
}

TargetType Minion::GetTargetType()
{
    if (m_skillIsReady)
    {
        // Buff 스킬의 경우 Target을 Minion으로
        if (m_skillData->skillType == SkillType::Buff)
        {
            return TargetType::Minion;
        }
    }

    return TargetType::Enemy;
}

void Minion::OnKillEvents(const std::shared_ptr<Room>& room)
{
    if (!m_skillData)
        return;

    for (int i=0; i < m_killEvents.size(); i++)
    {
        m_killEvents[i]->OnKill(
            shared_from_this(),
            m_skillData->killEvents[i],
            room
        );
    }
}

void Minion::OnHitEvents(const std::shared_ptr<Room>& room, const std::shared_ptr<Enemy>& enemy)
{
    if (!room || !enemy || !m_skillData)
        return;

    for (int i = 0; i < m_hitEvents.size(); ++i)
    {
        if (i >= static_cast<int>(m_skillData->hitEvents.size()))
            break;

        m_hitEvents[i]->OnHit(
            shared_from_this(),
            enemy,
            m_skillData->hitEvents[i],
            room
        );
    }
}