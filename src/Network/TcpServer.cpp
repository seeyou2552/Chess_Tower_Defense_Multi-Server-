#include "Network/TcpServer.h"
#include "Core/Logger.h"
#include "Network/Session.h"
#include "Game/GameServer.h"

#include <memory>
#include <iostream>


TcpServer::TcpServer(
    asio::io_context& ioContext,
    GameServer& server,
    PacketDispatcher& dispatcher,
    const ServerConfig& config
)
    :
    m_ioContext(ioContext),
    m_acceptor(
        ioContext,
        asio::ip::tcp::endpoint(
            asio::ip::tcp::v4(),
            config.port
        )
    ),
    m_server(server),
    m_dispatcher(dispatcher)
{
}

bool TcpServer::Start(uint16_t port)
{
    Logger::GetInstance().Info("Listening...");

    StartAccept();

    return true;
}

void TcpServer::StartAccept()
{
    auto socket =
        std::make_shared<asio::ip::tcp::socket>(
            m_ioContext
        );

    m_acceptor.async_accept(
        *socket,

        [this, socket]
        (
            std::error_code ec
        )
        {
            Logger::GetInstance().Info("Accept Callback");

            if(ec)
            {
                Logger::GetInstance().Error(
                    ec.message()
                );

                StartAccept();
                return;
            }

            Logger::GetInstance().Info(
                "Client Connected"
            );

            auto session =
                std::make_shared<Session>(
                    std::move(*socket),
                    m_server,
                    m_dispatcher
                );

            session->Start();

            StartAccept();
        }
    );
}