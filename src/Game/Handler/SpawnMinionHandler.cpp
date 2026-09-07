#include "Game/Handler/SpawnMinionHandler.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/MinionPacket.h"
#include "Core/Logger.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"
#include "Game/Player.h"
#include "Game/Room.h"
#include "Game/SpawnManager.h"

void SpawnMinionHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    PacketReader reader(packet, size);

    SpawnMinionRequest request;
    reader.Read(request);

    auto player = session->GetPlayer();
    if (player == nullptr) return;

    auto room = player->GetRoom();
    if (room == nullptr) return;

    SpawnMinionResponse response;

    response.errCode =
        room->SpawnMinion(
            request.minionId,
            player,
            request.x,
            request.y
        );
    response.currentGold = player->GetGold();

    PacketWriter writer(
        PacketType::SpawnMinionResponse
    );

    writer.Write(response);

    writer.Finish();

    session->Send(
        writer.GetBuffer()
    );
}