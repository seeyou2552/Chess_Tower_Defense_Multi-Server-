#pragma once
#include <cstdint>
#include "Game/Handler/IPacketHandler.h"


class SpawnMinionHandler
    : public IPacketHandler
{

public:

    void Handle(
        std::shared_ptr<Session> session,
        const char* packet,
        uint16_t size
    );

};