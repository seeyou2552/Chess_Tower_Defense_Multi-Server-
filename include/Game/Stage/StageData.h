#pragma once

#include <vector>
#include <cstdint>

#include "Game/Math/Vector3.h"

struct EnemySpawnData
{
    uint32_t enemyId;
    uint32_t count;
    float spawnDelay;
};

struct WaveData
{
    std::vector<EnemySpawnData> enemyList;
};

struct PathData
{
    std::vector<Vector3> waypoints;
};

struct StageData
{
    uint32_t stageId;

    std::vector<PathData> paths;

    uint32_t clearGold;

    std::vector<WaveData> waves;
};