#include "Game/Stage/StageManager.h"
#include "Game/Math/Vector3.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Core/Logger.h"

using json = nlohmann::json;

bool StageManager::Load(
    const std::string& file
)
{
    std::ifstream fs(file);

    if (!fs.is_open())
        return false;

    json root;

    fs >> root;

    for (auto& stageJson : root["Stages"])
    {
        auto stage =
            std::make_shared<StageData>();

        stage->stageId =
            stageJson["StageId"];

        stage->clearGold =
            stageJson["ClearGold"];

        //
        // Path
        //

        for (auto& pathJson : stageJson["Paths"])
        {
            PathData path;

            for (auto& pointJson : pathJson["Waypoints"])
            {
                Vector3 point;

                point.x = pointJson["x"];
                point.y = pointJson["y"];
                point.z = pointJson["z"];

                path.waypoints.push_back(point);
            }

            stage->paths.push_back(path);
        }

        //
        // Wave
        //

        for (auto& waveJson : stageJson["Wave"])
        {
            WaveData wave;

            for (auto& enemyJson : waveJson["EnemyList"])
            {
                EnemySpawnData enemy;

                enemy.enemyId =
                    enemyJson["EnemyId"];

                enemy.count =
                    enemyJson["Count"];

                enemy.spawnDelay =
                    enemyJson["SpawnDelay"];

                wave.enemyList.push_back(enemy);
            }

            stage->waves.push_back(wave);
        }

        m_stages.emplace(
            stage->stageId,
            stage
        );
    }

    return true;
}

std::shared_ptr<StageData>
StageManager::GetStage(
    uint32_t stageId
)
{
    auto iter =
        m_stages.find(stageId);

    if (iter == m_stages.end())
        return nullptr;

    return iter->second;
}

std::shared_ptr<StageTileData>
StageManager::GetStageTile(
    uint32_t stageId
)
{
    auto iter =
        m_allStageMaps.find(stageId);

    if (iter == m_allStageMaps.end())
        return nullptr;

    return std::make_shared<StageTileData>(iter->second);
}


bool StageManager::LoadAllStageTiles()
{
    const std::filesystem::path rootPath = "./";

    if (!std::filesystem::exists(rootPath))
    {
        Logger::GetInstance().Error(
            "Stage Tile Path Not Found : " +
            rootPath.string()
        );
        return false;
    }

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(rootPath))
        {
            if (!entry.is_regular_file())
                continue;

            std::string filename =
                entry.path().filename().string();

            if (filename.rfind("Stage_", 0) != 0)
                continue;

            if (filename.find("_Tiles.json") == std::string::npos)
                continue;

            Logger::GetInstance().Info(
                "Load Tile File : " + filename
            );

            uint32_t stageId =
                ExtractStageIdFromFilename(filename);

            std::ifstream fs(entry.path());

            if (!fs.is_open())
            {
                Logger::GetInstance().Error(
                    "Open Failed : " +
                    entry.path().string()
                );
                continue;
            }
            
            try
            {
                json tileRoot;
                fs >> tileRoot;

                StageTileData tileData;
                tileData.stageId = stageId;

                if (!tileRoot.contains("buildableTiles"))
                {
                    Logger::GetInstance().Warning(
                        filename + " has no Tiles field."
                    );
                    continue;
                }
                
                for (const auto& tileJson : tileRoot["buildableTiles"])
                {
                    Vector3Int pos{};

                    pos.x =
                        static_cast<int>(
                            tileJson["x"].get<double>() * 2
                            );

                    pos.y =
                        static_cast<int>(
                            tileJson["y"].get<double>() * 2
                            );

                    pos.z =
                        static_cast<int>(
                            tileJson["z"].get<double>()
                            );

                    tileData.buildableTiles.insert(pos);
                }

                m_allStageMaps[stageId] = tileData;

            }
            catch (const std::exception& e)
            {
                Logger::GetInstance().Error(
                    "JSON Parse Failed : "
                    + filename
                    + " / "
                    + e.what()
                );
            }
        }

        Logger::GetInstance().Info(
            "Stage Tile Loading Complete"
        );

        return true;
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Error(
            "LoadAllStageTiles Exception : "
            + std::string(e.what())
        );

        return false;
    }
}

uint32_t StageManager::ExtractStageIdFromFilename(const std::string& filename)
{
    uint32_t stageId = 0;
    // 예: "Stage_1_Tiles.json" 이라는 문자열에서 숫자 부분(%u)을 파싱합니다.
    std::sscanf(filename.c_str(), "Stage_%u_Tiles.json", &stageId);
    return stageId;
}