#pragma once

#include <cstdint>

struct PingRequest
{
    uint64_t tick;
};

struct PingResponse
{
    uint64_t tick;
};