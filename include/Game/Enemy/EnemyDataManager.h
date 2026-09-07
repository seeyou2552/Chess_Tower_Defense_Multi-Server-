#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>

#include "Game/Enemy/EnemyData.h"

class EnemyDataManager
{
public:

    static EnemyDataManager& GetInstance();

    bool Load(const std::string& file);

    const EnemyData* GetData(uint32_t id)
    {
        auto iter =
            m_datas.find(id);

        if (iter == m_datas.end())
            return nullptr;

        return iter->second.get();
    }

private:

    std::unordered_map<
        uint32_t,
        std::shared_ptr<EnemyData>
    > m_datas;
};