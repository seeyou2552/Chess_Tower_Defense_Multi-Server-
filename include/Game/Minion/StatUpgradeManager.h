#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>

#include "Game/Minion/Minion.h"
#include "Game/Minion/StatUpgrade.h"

class StatUpgradeManager
{
public:

    static StatUpgradeManager& GetInstance();

    void Init();
    
    void RegisterEvent(int id, std::shared_ptr<IStatUpgrade> eventObj) {
        m_eventMap[id] = eventObj;
    }

    void ExecuteEvent(int id, std::shared_ptr<Minion>& minion, float value) {
        auto it = m_eventMap.find(id);
        if (it != m_eventMap.end()) {
            it->second->OnUpgrade(minion, value);
        }
    }

private :

    std::unordered_map<
        int, 
        std::shared_ptr<IStatUpgrade>
    > 
        m_eventMap;
};