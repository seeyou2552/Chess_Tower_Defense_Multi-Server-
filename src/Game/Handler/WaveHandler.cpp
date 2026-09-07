#include "Game/Handler/WaveHandler.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/WavePacket.h"
#include "Core/Logger.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"
#include "Game/Player.h"

void WaveHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    PacketReader reader(packet, size);

    StartWaveRequest request;
    reader.Read(request);

    std::shared_ptr<Player> player = session->GetPlayer();
    if (player == nullptr) return;

    std::shared_ptr<Room> room = player->GetRoom();
    if (room == nullptr) return;

    if (room->PlayerReadyCheck(player))
    {
        // Wave 시작
        room->WaveStart();

        StartWaveNotify notify;

        PacketWriter writer(
            PacketType::StartWaveNotify
        );

        writer.Write(notify);

        writer.Finish();

        room->Broadcast(writer.GetBuffer());
    }

    
}