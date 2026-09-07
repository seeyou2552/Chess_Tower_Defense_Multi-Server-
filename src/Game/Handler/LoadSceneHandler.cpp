#include "Game/Handler/LoadSceneHandler.h"

#include "Game/Room.h"
#include "Game/Player.h"

#include "Network/Session.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Packet/LoadScenePacket.h"

#include "Core/Logger.h"

void LoadSceneHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    if (!session) return;

    auto player = session->GetPlayer();
    if (!player)
    {
        Logger::GetInstance().Error("LoadSceneHandler: Player is nullptr");
        return;
    }

    auto room = player->GetRoom();
    if (!room)
    {
        Logger::GetInstance().Error("LoadSceneHandler: Room is nullptr");
        return;
    }

    // 플레이어 전원 준비 확인
    if (room->PlayerReadyCheck(player))
    {
        PacketWriter writer(PacketType::LoadedScene);

        LoadedScene response{};
        response.errCode = ErrorCode::None;

        writer.Write(response);
        writer.Finish();

        room->Broadcast(
            writer.GetBuffer()
        );

        Logger::GetInstance().Info("all players loaded");
    }

}