#pragma once

#include <cstdint>


struct RegisterRequest
{
    char loginId[32];
    char password[32];
};


struct RegisterResponse
{
    char uuid[64];
    ErrorCode errCode;
};