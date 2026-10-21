#pragma once

#include <cstdint>

struct ServerConfig
{
    uint16_t port = 7777;

    uint32_t maxConnections = 1000;

    uint32_t maxPacketsPerSecond = 100;
    uint32_t packetBlockDurationSeconds = 5;
};