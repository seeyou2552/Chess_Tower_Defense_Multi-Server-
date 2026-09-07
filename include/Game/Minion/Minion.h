#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <vector>

#include "Game/Skill/SkillData.h"
#include "Game/Minion/MinionStat.h"
#include "Game/Minion/StatUpgrade.h"
#include "Game/Math/Vector3.h"
#include "Game/Skill/SkillTypes.h"
#include "Game/Skill/SkillStrategy.h"
#include "Game/Skill/BuffSkill.h"

class Enemy;
class MinionData;

class Minion : public std::enable_shared_from_this<Minion>
{
public:

    Minion();

    // CallBack
    using OnAttackCallback = std::function<void(
        uint32_t,
        std::vector<std::shared_ptr<Enemy>>
    )>;

    using OnEnemyTargetSkillCallback = std::function<void(
        uint32_t,
        std::vector<std::shared_ptr<Enemy>>
    )>;

    using OnMinionTargetSkillCallback = std::function<void(
        uint32_t,
        std::vector<std::shared_ptr<Minion>>
        )>;

    void Init(
        uint32_t instanceId,
        uint32_t orderId,
        const MinionData& data,
        int x,
        int y
    );

    void Reset();

    void Update(float deltaTime);

    
    const uint32_t& GetInstanceId() const
    {
        return m_instanceId;
    }

    const uint32_t& GetDataId() const
    {
		return m_dataId;
    }

    const uint32_t& GetOrderId() const
    {
        return m_orderId;
    }

    const Vector3& GetPosition() const
    {
        return m_position;
    }

    const float& GetAttackRange() const
    {
        return m_stat.GetCurrent().attackRange;
    }

    const DeliveryType& GetDeliveryType() const
    {
        return m_stat.GetCurrent().attackSpec.deliveryType;
    }

    const int& GetPower() const
    {
        return m_stat.GetCurrent().power;
    }

    const float& GetAttackCooldown() const
    {
        return m_stat.GetCurrent().attackCooldown;
    }

    const int& GetCurrentAC() const
    {
        return m_stat.GetCurrent().currentAC;
    }

    const int& GetMaxAC() const
    {
        return m_stat.GetCurrent().maxAC;
    }

    const int& GetMaxTargets() const;

    const MinionBaseStat& GetBaseStat() const
    {
        return m_stat.GetBase();
    }

    const MinionCurrentStat& GetCurrentStat() const
    {
        return m_stat.GetCurrent();
    }

    void AddPowerBuff(const BuffData& buffData)
    {
        m_stat.AddPowerBuff(buffData);
    }

    void AddAttackSpeedBuff(const BuffData& buffData)
    {
        m_stat.AddAttackSpeedBuff(buffData);
    }

    void AddMaxTargetsBuff(BuffData& buffData)
    {
        m_stat.AddAttackSpeedBuff(buffData);
    }

    void AddAttackSpecBuff(const DefaultAttackSpec& newSpec, float duration)
    {
        m_stat.AddAttackSpecBuff(newSpec, duration);
    }

    void ClearBuffs()
    {
        m_stat.ClearBuffs();
        m_stat.Recalculate();
    }

    void AddBasePower(int value)
    {
        m_stat.AddBasePower(value);
    }

    void AddBaseAttackCooldown(float value)
    {
        m_stat.AddBaseAttackCooldown(value);
    }

    void AddBaseMaxAC(int value)
    {
        m_stat.AddBaseMaxAC(value);
    }

    void AddBaseSkillMaxTargets(int value)
    {
        m_stat.AddBaseSkillMaxTargets(value);
    }

    void AddBaseRange(float value)
    {
        m_stat.AddBaseRange(value);
    }

    // Atk Spec
    const SearchScope& GetSearchScope() const
    {
        if (m_skillIsReady)
            return m_skillData->searchScope;

        return m_stat.GetCurrent().attackSpec.searchScope;
    }

    const ProjectileType& GetProjectileType() const
    {
        if (m_skillIsReady)
			return m_skillData->projectileType;
        
        return m_stat.GetCurrent().attackSpec.projectileType;
    }

    const ProjectileHitType& GetProjectileHitType() const
    {
        if (m_skillIsReady)
            return m_skillData->projectileHitType;

        return m_stat.GetCurrent().attackSpec.projectileHitType;
    }

    const float& GetProjectileSpeed() const
    {
        if (m_skillIsReady)
            return m_skillData->projectileSpeed;

        return m_stat.GetCurrent().attackSpec.projectileSpeed;
    }

    const float& GetProjectileDuration() const
    {
        if (m_skillIsReady)
            return m_skillData->projectileDuration;

        return m_stat.GetCurrent().attackSpec.projectileDuration;
    }

    const int& GetSellPrice() const
    {
        return m_sellPrice;
    }

    void SetPosition(const Vector3Int& pos)
    {
        m_position.x = pos.x / 2.0f;
        m_position.y = pos.y / 2.0f;
    }

    const float& GetAtkRange()
    {
        return m_stat.GetCurrent().attackRange;
    }

    UpgradeSlot* GetUpgradeSlot(int upgradeId)
    {
        auto iter =
            m_upgradeSlots.find(upgradeId);

        if (iter == m_upgradeSlots.end())
            return nullptr;

        return &(iter->second);
    }

    const SkillData* GetSkillData() const
    {
        return m_skillData;
    }

    const std::vector<BuffData>& GetBuffData() const
    {
        return m_skillData->buffList;
    }

    std::shared_ptr<IEnemyTargetSkillStrategy>& GetEnemyTargetSkillStrategy()
    {
        return m_enemyTargetSkillStrategy;
    }

    std::shared_ptr<IMinionTargetSkillStrategy>& GetMinionTargetSkillStrategy()
    {
        return m_minionTargetSkillStrategy;
    }

    void SetOnDefaultAttackCallback(OnAttackCallback callback)
    {
        m_onDefaultAttack = callback;
    }

    void SetOnEnemyTargetSkillCallback(OnEnemyTargetSkillCallback callback)
    {
        m_onEnemyTargetSkill = callback;
    }

    void SetOnMinionTargetSkillCallback(OnMinionTargetSkillCallback callback)
    {
        m_onMinionTargetSkill = callback;
    }

    void ResetAC()
    {
        m_stat.ResetCurrentAC();
    }

	void ResetAttackTimer()
	{
		m_atkTimer = 0.0f;
	}

    TargetType GetTargetType();

    const bool& AttackReadyCheck()
    {
        return m_attackIsReady;
    };

    const bool& CheckChangedDefaultAttack()
    {
        return m_stat.CheckChangedDefaultAttack();
    }

    void Recalculate()
    {
        m_stat.Recalculate();
    }

    // Attack
    void OnEnemyTarget(std::vector<std::shared_ptr<Enemy>>& targets);
    void OnMinionTarget(std::vector<std::shared_ptr<Minion>> targets);
    
    // Hit / Kill Event
    void OnKillEvents(const std::shared_ptr<Room>& room);
    void OnHitEvents(const std::shared_ptr<Room>& room, const std::shared_ptr<Enemy>& enemy);

private:
    
    float m_atkTimer = 0;

    uint32_t m_instanceId = 0;
    uint32_t m_dataId = 0;
    uint32_t m_orderId = 0;

    Vector3 m_position;

    MinionStat m_stat;
    std::unordered_map<int, UpgradeSlot> m_upgradeSlots;
    
    int m_sellPrice;

    bool m_attackIsReady = false;
    bool m_skillIsReady = false;

    bool m_active = false;

    OnAttackCallback m_onDefaultAttack;
    OnEnemyTargetSkillCallback m_onEnemyTargetSkill;
    OnMinionTargetSkillCallback m_onMinionTargetSkill;

    const SkillData* m_skillData = nullptr;
    std::shared_ptr<IEnemyTargetSkillStrategy> m_enemyTargetSkillStrategy;
    std::shared_ptr<IMinionTargetSkillStrategy> m_minionTargetSkillStrategy;

    std::vector<std::shared_ptr<IKillEvent>> m_killEvents;
    std::vector<std::shared_ptr<IHitEvent>> m_hitEvents;
};