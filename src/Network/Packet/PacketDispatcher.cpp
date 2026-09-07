#include "Network/Packet/PacketDispatcher.h"
#include "Network/Packet/PacketType.h"

#include "Network/Session.h"

#include "Core/Logger.h"


void PacketDispatcher::Register(
    PacketType type,
    HandlerFunc handler
)
{
    m_handlers[type] = handler;
}



void PacketDispatcher::Dispatch(
    PacketType type,
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    auto iter =
        m_handlers.find(type);

    if(iter == m_handlers.end())
    {
        Logger::GetInstance()
            .Warning("Unknown Packet");

        return;
    }

    iter->second(
        session,
        packet,
        size
    );
}