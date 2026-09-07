#pragma once

#include <cstdint>

#include "Network/ErrorCode.h"

struct JoinRoomRequest
{
    uint32_t stageId;
};

struct JoinRoomResponse
{
	ErrorCode errCode;
};