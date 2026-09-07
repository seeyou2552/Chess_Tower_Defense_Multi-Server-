#include "Game/Handler/UseGoldHandler.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/PlayerPacket.h"
#include "Core/Logger.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"
#include "Game/Player.h"

void UseGoldHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    PacketReader reader(packet, size);

    UseGoldRequest request;
    reader.Read(request);

    std::shared_ptr<Player> player = session->GetPlayer();
    if (player == nullptr) return;

    std::shared_ptr<Room> room = player->GetRoom();
    if (room == nullptr) return;

    UseGoldResponse response;

    // 골드 부족확인 및 차감
    if (!player->TryUseGold(request.price))
    {
        response.errCode = ErrorCode::GoldNotEnough;
    }

    else
    {
        response.errCode = ErrorCode::None;
    }

    PacketWriter writer(
        PacketType::UseGoldResponse
    );

    writer.Write(response);

    writer.Finish();

    session->Send(
        writer.GetBuffer()
    );
}