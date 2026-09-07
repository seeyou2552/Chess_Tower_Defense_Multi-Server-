#include "Game/Enemy/EnemyDataManager.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Core/Logger.h"

using json = nlohmann::json;

EnemyDataManager& EnemyDataManager::GetInstance()
{
    static EnemyDataManager instance;
    return instance;
}

bool EnemyDataManager::Load(
    const std::string& file
)
{
    std::ifstream fs(file);

    if (!fs.is_open())
    {
        Logger::GetInstance().Error(
            "Enemy Data Open Failed : "
            + file
        );

        return false;
    }

    json root;

    try
    {
        fs >> root;

        for (const auto& enemyJson : root["Enemies"])
        {
            auto data = std::make_shared<EnemyData>();


            data->id = enemyJson["EnemyId"].get<uint32_t>();
            data->maxHp = enemyJson["MaxHP"].get<float>();
            data->attack = enemyJson["AttackDamage"].get<int>(); // 데이터 타입에 맞게 조절
            data->moveSpeed = enemyJson["MoveSpeed"].get<float>();
            data->reward = enemyJson["Reward"].get<int>();

            m_datas.emplace(
                data->id,
                data
            );

            Logger::GetInstance().Info(
                "Loaded Enemy : "
                + std::to_string(data->id)
            );
        }

    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Error(
            "Enemy JSON Parse Failed : "
            + std::string(e.what())
        );

        return false;
    }

    Logger::GetInstance().Info(
        "Enemy Data Loading Complete : "
        + std::to_string(
            m_datas.size()
        )
    );

    return true;
}