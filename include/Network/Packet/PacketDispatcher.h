#pragma once

#include <unordered_map>
#include <functional>
#include <memory>

#include "PacketType.h"

class Session;

class PacketDispatcher
{

public:

    using HandlerFunc = std::function<
    void(
        std::shared_ptr<Session>,
        const char*,
        uint16_t
    )
>;


public:

    void Register(
        PacketType type,
        HandlerFunc handler
    );


    void Dispatch(
        PacketType type,
        std::shared_ptr<Session> session,
        const char* packet,
        uint16_t size
    );


private:

    std::unordered_map
    <
        PacketType,
        HandlerFunc
    >
    m_handlers;

};