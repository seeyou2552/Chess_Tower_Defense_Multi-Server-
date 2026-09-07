#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>

#include "Game/Minion/MinionData.h"
#include "Game/Minion/StatUpgrade.h"

class MinionDataManager
{
public:
    
    static MinionDataManager& GetInstance();

    bool Load(const std::string& file);

    const MinionData* GetData(uint32_t id)
    {
        auto iter =
            m_datas.find(id);

        if (iter == m_datas.end())
            return nullptr;

        return iter->second.get();
    }

    std::unordered_map<int, UpgradeSlot> ParseUpgradeData(const std::string& rawData);

private:

    std::unordered_map<
        uint32_t,
        std::shared_ptr<MinionData>
    >
    m_datas;
};