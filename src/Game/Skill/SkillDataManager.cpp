#include "Game/Skill/SkillDataManager.h"
#include "Game/Skill/SkillTypes.h"
#include "Game/Skill/BuffSkill.h"
#include "Game/Skill/HitEvent.h"
#include "Game/Skill/KillEvent.h"


#include <fstream>
#include <nlohmann/json.hpp>

#include "Core/Logger.h"

using json = nlohmann::json;

SkillDataManager& SkillDataManager::GetInstance()
{
    static SkillDataManager instance;
    return instance;
}

bool SkillDataManager::Load(const std::string& file)
{
    std::ifstream fs(file);

    if (!fs.is_open())
    {
        Logger::GetInstance().Error(
            "Skill Data Open Failed : " + file
        );
        return false;
    }

    json root;

    try
    {
        fs >> root;

        // "Skills": [ ... ] 구조 형태 지원을 위해 root가 객체이고 Skills 키가 있는 경우와 배열인 경우 모두 대응
        const json& skillsArray = root.contains("Skills") ? root["Skills"] : root;

        for (auto& skillJson : skillsArray)
        {
            auto data = std::make_shared<SkillData>();

            data->id = skillJson["ID"].get<unsigned int>();
            data->skillType = skillJson["SkillType"].get<SkillType>();
            data->searchScope = skillJson["SearchScope"].get<SearchScope>();
            data->maxTargets = skillJson["MaxTargets"].get<int>();
            data->projectileType = skillJson["ProjectileType"].get<ProjectileType>();
            data->projectileHitType = skillJson["ProjectileHitType"].get<ProjectileHitType>();

            data->projectileSpeed = skillJson["ProjectileSpeed"].get<float>();
            data->projectileDuration = skillJson["ProjectileDuration"].get<float>();

            // BuffList 파싱 (ChangeDefaultAttackData 포함)
            if (skillJson.contains("BuffList") && skillJson["BuffList"].is_array())
            {
                for (const auto& buffJson : skillJson["BuffList"])
                {
                    BuffData buffData;

                    buffData.id = buffJson["BuffID"].get<int>();
                    buffData.value = buffJson["Value"].get<float>();
                    buffData.duration = buffJson["Duration"].get<float>();

                    // DefaultAttackData 파싱 (null이 아닐 때만 처리)
                    if (buffJson.contains("DefaultAttackData") && !buffJson["DefaultAttackData"].is_null())
                    {
                        const auto& atkJson = buffJson["DefaultAttackData"];
                        DefaultAttackSpec attackData;

                        attackData.deliveryType = atkJson["DeliveryType"].get<DeliveryType>();
                        attackData.searchScope = atkJson["SearchScope"].get<SearchScope>();
                        attackData.maxTargets = atkJson["maxTargets"].get<int>();
                        attackData.projectileType = atkJson["ProjectileType"].get<ProjectileType>();
                        attackData.projectileHitType = atkJson["ProjectileHitType"].get<ProjectileHitType>();
                        attackData.projectileSpeed = atkJson["ProjectileSpeed"].get<float>();
                        attackData.projectileDuration = atkJson["ProjectileDuration"].get<float>();

                        buffData.changeAttackSpec = attackData;
                    }

                    data->buffList.push_back(buffData);
                }
            }

            data->skillDamage = skillJson["SkillDamage"].get<int>();
            data->skillScale = skillJson["SkillScale"].get<float>();

            // HitEvents 배열 파싱
            if (skillJson.contains("HitEvents") && skillJson["HitEvents"].is_array())
            {
                for (auto& hitJson : skillJson["HitEvents"])
                {
                    HitEventData hitEvent;
                    hitEvent.id = hitJson["EventID"].get<unsigned int>();
                    hitEvent.duration = hitJson["Duration"].get<float>();
                    hitEvent.value = hitJson["Value"].get<float>();
                    data->hitEvents.push_back(hitEvent);
                }
            }

            // KillEvents 배열 파싱
            if (skillJson.contains("KillEvents") && skillJson["KillEvents"].is_array())
            {
                for (auto& killJson : skillJson["KillEvents"])
                {
                    KillEventData killEvent;
                    killEvent.id = killJson["EventID"].get<unsigned int>();
                    killEvent.duration = killJson["Duration"].get<float>();
                    killEvent.value = killJson["Value"].get<float>();
                    data->killEvents.push_back(killEvent);
                }
            }

            m_datas.emplace(data->id, data);

            Logger::GetInstance().Info(
                "Loaded Skill : " + std::to_string(data->id)
            );
        }
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Error(
            "Skill JSON Parse Failed : " + std::string(e.what())
        );
        return false;
    }

    Logger::GetInstance().Info(
        "Skill Data Loading Complete : " + std::to_string(m_datas.size())
    );

    return true;
}