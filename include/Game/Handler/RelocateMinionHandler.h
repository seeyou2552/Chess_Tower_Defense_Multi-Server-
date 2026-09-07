#pragma once
#include <cstdint>
#include "Game/Handler/IPacketHandler.h"


class RelocateMinionHandler
    : public IPacketHandler
{

public:

    void Handle(
        std::shared_ptr<Session> session,
        const char* packet,
        uint16_t size
    );

};