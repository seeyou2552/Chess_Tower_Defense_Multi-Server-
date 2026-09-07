#pragma once

#include <string>

#include "Network/ErrorCode.h"


struct UseGoldRequest
{
    uint32_t price;
};

struct UseGoldResponse
{
    uint32_t currentGold;
    ErrorCode errCode;
};

struct UpdateGold
{
    int currentGold;
};