#pragma once

#include <cstdint>

#include "Network/ErrorCode.h"

struct JoinRoomRequest
{
    int stageId;
};

struct JoinRoomResponse
{
	ErrorCode errCode;
};