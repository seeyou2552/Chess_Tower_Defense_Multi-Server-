#include "Game/Handler/RegisterHandler.h"

#include "Network/Session.h"
#include "Network/ErrorCode.h"
#include "Network/Packet/RegisterPacket.h"
#include "Network/Packet/PacketWriter.h"

#include "Game/GameServer.h"
#include "Game/AccountManager.h"

#include "Core/Logger.h"


void RegisterHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    RegisterRequest request{};

    memcpy(
        &request,
        packet,
        sizeof(RegisterRequest)
    );

    std::string loginId =
        request.loginId;

    std::string password =
        request.password;

    std::string uuid;

    Logger::GetInstance().Info(
        std::to_string(sizeof(RegisterResponse)));


    RegisterResponse response{};

    auto& server =
        session->GetServer();

    auto account =
        server.GetAccountManager()
        .Register(
            request.loginId,
            request.password,
            response.errCode
        );

    if(account)
    {
        strcpy(
            response.uuid,
            uuid.c_str()
        );
    }

    auto p = reinterpret_cast<unsigned char*>(&response);

    PacketWriter writer(
        PacketType::RegisterResponse
    );

    writer.Write(
        response
    );

    writer.Finish();

    session->Send(
        writer.GetBuffer()
    );


}