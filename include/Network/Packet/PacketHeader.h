#pragma once

#include <cstdint>

#include "Network/Packet/PacketType.h"

#pragma pack(push, 1)

struct PacketHeader
{
    uint16_t size;
    PacketType type;
};

#pragma pack(pop)