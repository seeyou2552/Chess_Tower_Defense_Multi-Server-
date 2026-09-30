#pragma once

#include <string>

#include "Network/ErrorCode.h"


struct StageStartResponse
{
    int gold;
    int orderId;
    ErrorCode errCode;
};

struct StageEndNotify
{
    uint32_t success;
};