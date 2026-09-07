#pragma once

#include <asio.hpp>
#include "Config/ServerConfig.h"

class GameServer;
class PacketDispatcher;

class TcpServer
{
public:
    explicit TcpServer(asio::io_context& ioContext);

    bool Start(uint16_t port);

    TcpServer(
        asio::io_context& ioContext,
        GameServer& server,
        PacketDispatcher& dispatcher,
        const ServerConfig& config
    );

private:
    void StartAccept();

private:
    asio::io_context& m_ioContext;
    asio::ip::tcp::acceptor m_acceptor;
    GameServer& m_server;
    PacketDispatcher& m_dispatcher;
};