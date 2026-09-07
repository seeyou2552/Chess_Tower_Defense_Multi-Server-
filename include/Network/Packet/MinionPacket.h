#pragma once

#include <string>
#include <vector>
#include <optional>

#include "Network/ErrorCode.h"
#include "Game/Skill/SkillTypes.h"
#include "Game/Skill/BuffSkill.h"

// Spawn
struct SpawnMinionRequest
{
    uint32_t minionId;
    int x;
    int y;
};

struct SpawnMinionResponse
{
	uint32_t currentGold;
    ErrorCode errCode;
};

struct SpawnMinionNotify
{
	uint32_t minionId;
	uint32_t instanceId;
	int orderId;
	int x;
	int y;
};

// Relocate
struct RelocateMinionRequest
{
	uint32_t instanceId;
	int x;
	int y;
};

struct RelocateMinionResponse
{
	uint32_t instanceId;
	ErrorCode errCode;
};

struct RelocateMinionNotify
{
	uint32_t instanceId;
	int x;
	int y;
};

// Sell
struct SellMinionRequest
{
	uint32_t instanceId;
};

struct SellMinionResponse
{
	int currentGold;
	ErrorCode errCode;
};

// Upgrade
struct UpgradeMinionRequest
{
	uint32_t instanceId;
	int upgradeId;
};

struct UpgradeMinionResponse
{
	int currentGold;
	ErrorCode errCode;
};

struct UpgradeMinionNotify
{
	uint32_t instanceId;
	int upgradeId;
	int upgradeLevel;
};

// Return
struct ReturnMinionNotify
{
	uint32_t instanceId;
};

// Attack

struct AttackNotify
{
	int atkCount;
};

