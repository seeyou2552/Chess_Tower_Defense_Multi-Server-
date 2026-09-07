#pragma once

#include <string>

#include "Network/ErrorCode.h"


struct LoginRequest
{
    char id[32];
    char password[32];
};

struct LoginResponse
{
    char uuid[64];
    uint32_t playerId;
    char accessToken[128];
    ErrorCode errCode;
};

