#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>

#include "Game/Minion/Minion.h"
#include "Game/Skill/BuffSkill.h"
#include "Game/Skill/HitEvent.h"
#include "Game/Skill/KillEvent.h"

class SkillManager
{
public:

    static SkillManager& GetInstance();

    void Init();

    // Buff
    void RegisterBuffSkill(int id, std::shared_ptr<IBuffSkillStrategy> eventObj) {
        m_buffSkill[id] = eventObj;
    }

    void ExecuteBuffSkill(int id, std::vector<std::shared_ptr<Minion>>& minions, const BuffData& data) {
        auto it = m_buffSkill.find(id);
        if (it != m_buffSkill.end()) {
            it->second->OnBuff(minions, data);
        }
    }

    // Kill
    void RegisterKillEvent(int id, std::shared_ptr<IKillEvent> eventObj) {
        m_killEvents[id] = eventObj;
    }

    void ExecuteKillEvent(
        int id, 
        std::shared_ptr<Minion>& minion,
        const KillEventData& data,
        std::shared_ptr<Room> room
    ) 
    {
        auto it = m_killEvents.find(id);
        if (it != m_killEvents.end()) {
            it->second->OnKill(minion, data, room);
        }
    }

    // Hit


private:

    std::unordered_map<
        int,
        std::shared_ptr<IBuffSkillStrategy>
    >
        m_buffSkill;

    std::unordered_map<
        int,
        std::shared_ptr<IKillEvent>
    >
        m_killEvents;

    std::unordered_map<
        int,
        std::shared_ptr<IHitEvent>
    >
        m_hitEvents;
};