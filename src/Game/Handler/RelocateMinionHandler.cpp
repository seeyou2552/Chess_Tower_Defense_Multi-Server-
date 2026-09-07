#include "Game/Handler/RelocateMinionHandler.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/MinionPacket.h"
#include "Core/Logger.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"
#include "Game/Player.h"
#include "Game/Room.h"
#include "Game/SpawnManager.h"

void RelocateMinionHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    PacketReader reader(packet, size);

    RelocateMinionRequest request;
    reader.Read(request);

    std::shared_ptr<Player> player = session->GetPlayer();
    if (player == nullptr) return;

    std::shared_ptr<Room> room = player->GetRoom();
    if (room == nullptr) return;

    RelocateMinionResponse response;

    auto orderId = player->GetId();

    response.instanceId = request.instanceId;
    response.errCode =
        room->RelocateMinion(
            request.instanceId,
            orderId,
            request.x,
            request.y
        );

    if (response.errCode == ErrorCode::InvalidMinionId)
    {
        Logger::GetInstance().Info("Id Error");
    }
    

    PacketWriter writer(
        PacketType::RelocateMinionResponse
    );

    writer.Write(response);

    writer.Finish();

    session->Send(
        writer.GetBuffer()
    );
}