#pragma once

#include <memory>
#include <vector>

class Session;

class IPacketHandler
{
public:

    virtual ~IPacketHandler() = default;


    virtual void Handle(
        std::shared_ptr<Session> session,
        const char* packet,
        uint16_t size
    ) = 0;
};