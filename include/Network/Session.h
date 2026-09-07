#pragma once

#include <asio.hpp>
#include <memory>
#include <array>
#include <deque>
#include <mutex>

#include "Network/Packet/PacketHeader.h"
#include "Network/Packet/ReceiveBuffer.h"

class GameServer;
class PacketDispatcher;
class Player;

class Session
    : public std::enable_shared_from_this<Session>
{
public:

    Session(
        asio::ip::tcp::socket socket,
        GameServer& server,
        PacketDispatcher& dispatcher
    );

    void Start();

    asio::ip::tcp::socket& GetSocket();

    GameServer& GetServer()
    {
        return m_server;
    }

    void SetPlayer(
        std::shared_ptr<Player> player
    )
    {
        m_player = player;
    }

    std::shared_ptr<Player> GetPlayer() const
    {
        return m_player;
    }

    void Send(
        const std::vector<char>& packet
    );

    void DoSend();

    void Disconnect();

private:

    void Read();

    void ProcessPackets();

private:

    asio::ip::tcp::socket m_socket;

    GameServer& m_server;

    PacketDispatcher& m_dispatcher;

    std::shared_ptr<Player> m_player;

    ReceiveBuffer m_receiveBuffer;

    std::array<char, 4096> m_tempBuffer;

    std::deque<std::vector<char>> m_sendQueue;
    std::mutex m_sendMutex;

    bool m_sending = false;
};