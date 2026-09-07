#include "Game/Handler/UpgradeMinionHandler.h"

#include "Network/Packet/PacketReader.h"
#include "Network/Packet/MinionPacket.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"

#include "Core/Logger.h"

#include "Game/Player.h"
#include "Game/Room.h"
#include "Game/Minion/Minion.h"
#include "Game/Minion/StatUpgradeManager.h"

void UpgradeMinionHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    try
    {
        PacketReader reader(packet, size);

        UpgradeMinionRequest request;
        reader.Read(request);

        // Session에 연결된 Player가 없는 경우
        auto player = session->GetPlayer();

        if (!player)
            return;

        // Player가 현재 Room에 속해있지 않은 경우
        auto room = player->GetRoom();

        if (!room)
            return;

        // Upgrade 요청 전달
        room->EnqueueCommand(
            [
                room,
                player,
                session,
                request
            ]()
            {
                // Upgrade Response 생성
                UpgradeMinionResponse response;

                // Minion 확인
                auto minion =
                    room->GetMinionManager().FindMinion(
                        request.instanceId
                    );

                // 존재하지 않는 Minion ID를 요청한 경우
                if (!minion)
                {
                    response.errCode =
                        ErrorCode::InvalidId;
                }

                // 해당 Minion의 소유자가 아닌 경우
                else if (
                    minion->GetOrderId() != player->GetId()
                    )
                {
                    response.errCode =
                        ErrorCode::InvalidOrderId;
                }

                // Minion이 정상이고 소유자도 일치하는 경우
                else
                {
                    response.errCode =
                        ErrorCode::None;
                }


                // Upgrade 가능 여부 확인
                if (response.errCode == ErrorCode::None)
                {
                    auto* slot =
                        minion->GetUpgradeSlot(
                            request.upgradeId
                        );

                    // 존재하지 않는 Upgrade ID를 요청한 경우
                    if (!slot)
                    {
                        response.errCode =
                            ErrorCode::InvalidId;
                    }

                    // 이미 최대 레벨에 도달한 경우
                    else if (
                        slot->currentLevel >= slot->maxLevel
                        )
                    {
                        response.errCode =
                            ErrorCode::AlreadyMaxLevel;
                    }

                    // 골드가 부족한 경우
                    else if (
                        !player->TryUseGold(slot->price)
                        )
                    {
                        response.errCode =
                            ErrorCode::GoldNotEnough;
                    }

                    // 모든 조건을 만족하여 Upgrade 가능한 경우
                    else
                    {
                        // 실제 Upgrade 처리
                        StatUpgradeManager::GetInstance()
                            .ExecuteEvent(
                                request.upgradeId,
                                minion,
                                slot->amount
                            );

                        // Upgrade Level 증가
                        slot->currentLevel++;

                        // Upgrade Notify
                        UpgradeMinionNotify notify;

                        notify.instanceId =
                            request.instanceId;

                        notify.upgradeId =
                            request.upgradeId;

                        notify.upgradeLevel =
                            slot->currentLevel;

                        PacketWriter notifyWriter(
                            PacketType::UpgradeMinionNotify
                        );

                        notifyWriter.Write(notify);
                        notifyWriter.Finish();

                        room->Broadcast(
                            notifyWriter.GetBuffer()
                        );
                    }
                }

                // 현재 Gold 설정
                response.currentGold =
                    player->GetGold();

                // Upgrade Response 전송
                PacketWriter writer(
                    PacketType::UpgradeMinionResponse
                );

                writer.Write(response);
                writer.Finish();

                session->Send(
                    writer.GetBuffer()
                );
            }
        );
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Info(
            std::string("UpgradeMinionHandler Exception: ")
            + e.what()
        );
    }
    catch (...)
    {
        Logger::GetInstance().Info(
            "UpgradeMinionHandler Unknown Exception"
        );
    }
}