#include "Game/Handler/PingHandler.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/PingPacket.h"
#include "Core/Logger.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"

void PingHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    PacketReader reader(packet, size);

    PingRequest request;
    reader.Read(request);

    Logger::GetInstance().Info(
        "Ping : " +
        std::to_string(request.tick)
    );

    PingResponse response;

    response.tick =
        request.tick;

    PacketWriter writer(
        PacketType::PingResponse
    );

    writer.Write(response);

    writer.Finish();

    session->Send(
        writer.GetBuffer()
    );
}