#include "Game/Handler/LoginHandler.h"

#include "Game/GameServer.h"
#include "Game/Player.h"
#include "Game/PlayerManager.h"
#include "Game/AccountManager.h"
#include "Game/Account.h"

#include "Network/Packet/LoginPacket.h"
#include "Network/Packet/PacketReader.h"
#include "Network/Packet/PacketWriter.h"
#include "Network/Session.h"

#include "Core/Logger.h"

#include <algorithm>

namespace
{
    std::string ReadFixedString(const char* value, size_t capacity)
    {
        const auto end = std::find(value, value + capacity, '\0');
        return std::string(value, end);
    }
}

void LoginHandler::Handle(
    std::shared_ptr<Session> session,
    const char* packet,
    uint16_t size
)
{
    PacketReader reader(
        packet,
        size
    );

    LoginRequest request{};
    reader.Read(request);

    LoginResponse response{};

    auto& server =
        session->GetServer();

    auto account =
        server.GetAccountManager()
        .Login(
            ReadFixedString(request.id, sizeof(request.id)),
            ReadFixedString(request.password, sizeof(request.password)),
            response.errCode
        );

    if(account)
    {
        auto player =
            server.GetPlayerManager()
            .CreatePlayer();

        player->SetSession(
            session
        );

        auto session = player->GetSession();

        if (!session)
        {
            Logger::GetInstance().Info("Session ERRRRRRRRRRRRR");
        }

        player->SetAccount(
            account
        );

        session->SetPlayer(
            player
        );

        strcpy(
            response.uuid,
            account->uuid.c_str()
        );
        
		response.playerId = player->GetId();
        snprintf(response.accessToken, sizeof(response.accessToken), "%s", player->GetToken().c_str());

        Logger::GetInstance()
        .Info(
            "Login Success : "
            + account->uuid
        );

        
    }
    else
    {
        response.uuid[0] = '\0';

        Logger::GetInstance()
        .Warning(
            "Login Failed"
        );
    }

    PacketWriter writer(
        PacketType::LoginResponse
    );

    writer.Write(
        response
    );

    writer.Finish();

    session->Send(
        writer.GetBuffer()
    );
}