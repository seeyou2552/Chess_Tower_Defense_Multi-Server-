#pragma once

#include <string>

#include "Network/ErrorCode.h"

struct SpawnEnemyNotify
{
	uint32_t enemyId;
	uint32_t instanceId;
	int pathIndex;
};

struct ArrivalEnemyNotify
{
	uint32_t instanceId;
	int wallHp;
};

struct DeadEnemyNotify
{
	int deathCount;
};
