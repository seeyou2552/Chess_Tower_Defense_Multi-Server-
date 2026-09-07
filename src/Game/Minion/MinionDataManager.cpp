#include "Game/Minion/MinionDataManager.h"
#include "Game/Minion/StatUpgrade.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Core/Logger.h"

using json = nlohmann::json;

MinionDataManager& MinionDataManager::GetInstance()
{
    static MinionDataManager instance;
    return instance;
}

bool MinionDataManager::Load(
    const std::string& file
)
{
    std::ifstream fs(file);

    if (!fs.is_open())
    {
        Logger::GetInstance().Error(
            "Minion Data Open Failed : "
            + file
        );

        return false;
    }

    json root;

    try
    {
        fs >> root;

        for (auto& minionJson : root)
        {
            auto data =
                std::make_shared<MinionData>();

            data->id =
                std::stoul(
                    minionJson["MinionIndex"]
                    .get<std::string>()
                );

            data->maxAC =
                std::stoi(
                    minionJson["MaxAC"]
                    .get<std::string>()
                );

            data->attackRange =
                std::stof(
                    minionJson["AttackRange"]
                    .get<std::string>()
                );

            data->attackPower =
                std::stoi(
                    minionJson["AttackPower"]
                    .get<std::string>()
                );

            data->attackCooldown =
                std::stof(
                    minionJson["AttackCooldown"]
                    .get<std::string>()
                );

            data->cost =
                std::stoi(
                    minionJson["Cost"]
                    .get<std::string>()
                );

            data->skillId =
                std::stoul(
                    minionJson["SkillID"]
                    .get<std::string>()
                );

            data->minionType =
                minionJson["MinionType"];

            // DeliveryType 파싱
            std::string deliveryTypeStr =
                minionJson["DeliveryType"]
                .get<std::string>();
            data->deliveryType =
                (deliveryTypeStr == "Instant") 
                    ? DeliveryType::Instant 
                    : DeliveryType::Projectile;

            // SearchScope 파싱
            std::string searchScopeStr =
                minionJson["SearchScope"]
                .get<std::string>();
            data->searchScope =
                (searchScopeStr == "SingleTarget")
                    ? SearchScope::SingleTarget
                    : SearchScope::MultipleTarget;

            // MaxTargets 파싱
            data->maxTargets =
                std::stoi(
                    minionJson["MaxTargets"]
                    .get<std::string>()
                );

            // ProjectileType 파싱
            std::string projectileTypeStr =
                minionJson["ProjectileType"]
                .get<std::string>();
            data->projectileType =
                (projectileTypeStr == "Chase")
                    ? ProjectileType::Chase
                    : ProjectileType::Straight;

            // ProjectileHitType 파싱
            std::string projectileHitTypeStr =
                minionJson["ProjectileHitType"]
                .get<std::string>();
            data->projectileHitType =
                (projectileHitTypeStr == "Impact")
                    ? ProjectileHitType::Impact
                    : ProjectileHitType::None;

            // Projectile 속도/지속시간 파싱
            data->projectileSpeed =
                std::stof(
                    minionJson["ProjectileSpeed"]
                    .get<std::string>()
                );

            data->projectileDuration =
                std::stof(
                    minionJson["ProjectileDuration"]
                    .get<std::string>()
                );

            // UpgradeData 파싱
            data->upgradeSlots =
                ParseUpgradeData(
                    minionJson["UpgradeData"]
                    .get<std::string>()
                );

            m_datas.emplace(
                data->id,
                data
            );


            Logger::GetInstance().Info(
                "Loaded Minion : "
                + std::to_string(data->id)
            );
        }

    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Error(
            "Minion JSON Parse Failed : "
            + std::string(e.what())
        );

        return false;
    }

    Logger::GetInstance().Info(
        "Minion Data Loading Complete : "
        + std::to_string(
            m_datas.size()
        )
    );

    return true;
}

std::unordered_map<int, UpgradeSlot> MinionDataManager::ParseUpgradeData(const std::string& rawData) {
    std::unordered_map<int, UpgradeSlot> slots;
    std::stringstream ss(rawData);
    std::string itemToken;

    // 세미콜론(;) 단위로 분리 (각 업그레이드 항목)
    while (std::getline(ss, itemToken, ';')) {
        if (itemToken.empty()) continue;

        std::stringstream itemSs(itemToken);
        std::string field;
        std::vector<std::string> fields;

        // 파이프(|) 단위로 분리 (id, maxLevel, amount, price)
        while (std::getline(itemSs, field, '|')) {
            fields.push_back(field);
        }

        if (fields.size() == 4) {
            int id = std::stoi(fields[0]);
            UpgradeSlot slot;
            slot.maxLevel = std::stoi(fields[1]);
            slot.amount = std::stof(fields[2]);
            slot.price = std::stoi(fields[3]);
            slot.currentLevel = 0; // 초기 레벨

            slots[id] = slot;
        }
    }
    return slots;
}