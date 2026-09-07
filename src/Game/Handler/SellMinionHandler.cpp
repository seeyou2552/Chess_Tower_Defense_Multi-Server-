#include "Game/Handler/SellMinionHandler.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/MinionPacket.h"
#include "Core/Logger.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"
#include "Game/Player.h"
#include "Game/Room.h"
#include "Game/Minion/Minion.h"

void SellMinionHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    try
    {
        PacketReader reader(packet, size);

        SellMinionRequest request;
        reader.Read(request);

        auto player = session->GetPlayer();
        if (player == nullptr) return;

        auto room = player->GetRoom();
        if (room == nullptr) return;

        SellMinionResponse response;

        auto minion = room->GetMinionManager().FindMinion(request.instanceId);
        if (!minion)
            response.errCode = ErrorCode::InvalidId;

        else if (minion->GetOrderId() != player->GetId())
            response.errCode = ErrorCode::InvalidOrderId;

        else
            response.errCode = ErrorCode::None;

        // 플레이어 골드 추가 및 Minion Return
        if (response.errCode == ErrorCode::None)
        {
            player->AddGold(minion->GetSellPrice());

            room->ReturnMinion(request.instanceId);
        }

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
    catch (const std::exception& e)
    {
        // 비정상적인 패킷이나 파싱 에러로 서버가 꺼지는 것을 막습니다.
        Logger::GetInstance().Info(std::string("SellMinionHandler Exception: ") + e.what());

        // 필요하다면 여기서 해당 세션을 강제로 끊거나 에러 응답을 보낼 수 있습니다.
    }
    catch (...)
    {
        Logger::GetInstance().Info("SellMinionHandler Unknown Exception");
    }
}