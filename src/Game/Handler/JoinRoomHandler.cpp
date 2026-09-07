#include "Game/Handler/JoinRoomHandler.h"

#include "Network/Session.h"

#include "Game/GameServer.h"
#include "Game/RoomManager.h"
#include "Game/Room.h"
#include "Game/Player.h"

#include "Network/Packet/RoomPacket.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/PacketWriter.h"

#include "Core/Logger.h"


void JoinRoomHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    Logger::GetInstance().Info("Handle Start");
    PacketReader reader(
        packet,
        size
    );

    JoinRoomRequest request{};

    reader.Read(request);

    auto player =
        session->GetPlayer();

    JoinRoomResponse response{};

    if(!player)
    {
        response.errCode = ErrorCode::None;
    }
    else
    {
        auto& server =
            session->GetServer();


        auto room =
            server.GetRoomManager()
                .GetRoom(
                    request.stageId
                );


        if(room)
        {
            room->Enter(player);

			response.errCode = ErrorCode::None;

            Logger::GetInstance().Info(
                "Join Room : " +
                std::to_string(room->GetId())
            );
        }
        else
        {
			room = server.GetRoomManager()
				.CreateRoom(request.stageId);

            Logger::GetInstance().Info("Create");

            room->Enter(player);

            response.errCode = ErrorCode::None;

            Logger::GetInstance().Info(
                "Join Room : " +
                std::to_string(room->GetId())
            );
        }
    }


    //PacketWriter writer(
    //    PacketType::JoinRoomResponse
    //);


    //writer.Write(response);

    //writer.Finish();


    //session->Send(
    //    writer.GetBuffer()
    //);

    Logger::GetInstance().Info("Join Room Handler Complete");
}