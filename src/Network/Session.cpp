#include "Network/Session.h"
#include "Core/Logger.h"
#include "Game/GameServer.h"
#include "Network/Packet/PacketDispatcher.h"
#include "Game/Player.h"

Session::Session(
    asio::ip::tcp::socket socket,
    GameServer& server,
    PacketDispatcher& dispatcher
)
    :
    m_socket(std::move(socket)),
    m_server(server),
    m_dispatcher(dispatcher)
{
}

asio::ip::tcp::socket&
Session::GetSocket()
{
    return m_socket;
}

void Session::Start()
{
    Read();
}

void Session::Read()
{
    Logger::GetInstance().Info("Read() 등록");

    auto self = shared_from_this();

    m_socket.async_read_some(
        asio::buffer(m_tempBuffer),

        [this, self]
        (
            std::error_code ec,
            std::size_t bytesTransferred
        )
        {
            Logger::GetInstance().Info("Read Callback");

            if(ec)
            {
                Logger::GetInstance().Info(
                    "Client Disconnected"
                );

                auto player =
                    GetPlayer();

                if(player)
                {
                    m_server
                        .GetPlayerManager()
                        .RemovePlayer(
                            player->GetId()
                        );
                }

                return;
            }

            Logger::GetInstance().Info(
                "Received : " +
                std::to_string(bytesTransferred) +
                " bytes"
            );

            m_receiveBuffer.Append(
                m_tempBuffer.data(),
                bytesTransferred
            );

            Logger::GetInstance().Info(
                "ReceiveBuffer : " +
                std::to_string(m_receiveBuffer.Size())
            );

            ProcessPackets();

            Read();
        }
    );
}

void Session::ProcessPackets()
{
    while (true)
    {
        if (m_receiveBuffer.Size() < sizeof(PacketHeader))
            return;

        auto header =
            reinterpret_cast<const PacketHeader*>(
                m_receiveBuffer.Data());

        if (m_receiveBuffer.Size() < header->size)
            return;

        try
        {
            m_dispatcher.Dispatch(
                header->type,
                shared_from_this(),
                m_receiveBuffer.Data() + sizeof(PacketHeader),
                header->size - sizeof(PacketHeader)
            );
        }
        
        catch (const std::exception& e)
        {
            Logger::GetInstance().Info("Packet Dispatch Unknown Exception: " + std::string(e.what()));
        }

        m_receiveBuffer.Remove(header->size);
    }
}

void Session::Send(
    const std::vector<char>& packet
)
{
    bool shouldStart = false;

    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        m_sendQueue.push_back(packet);

        if (!m_sending)
        {
            m_sending = true;
            shouldStart = true;
        }
    }

    if (!shouldStart)
    {
        return;
    }

    DoSend();
}

void Session::DoSend()
{
    std::vector<char>* packet = nullptr;

    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        if (m_sendQueue.empty())
        {
            m_sending = false;
            return;
        }

        packet = &m_sendQueue.front();
    }

    auto self =
        shared_from_this();

    asio::async_write(
        m_socket,
        asio::buffer(
            *packet
        ),

        [this, self]
        (
            std::error_code ec,
            std::size_t
        )
        {
            bool shouldContinue = false;

            {
                std::lock_guard<std::mutex> lock(m_sendMutex);

                if (!m_sendQueue.empty())
                {
                    m_sendQueue.pop_front();
                }

                if (ec)
                {
                    m_sendQueue.clear();
                    m_sending = false;
                }
                else if (!m_sendQueue.empty())
                {
                    shouldContinue = true;
                }
                else
                {
                    m_sending = false;
                }
            }

            if(ec)
            {
                Logger::GetInstance()
                    .Error(
                        "Send Failed"
                    );

                return;
            }

            if (shouldContinue)
            {
                DoSend();
            }
        }
    );
}

void Session::Disconnect()
{
    auto player = m_player;

    if(player)
    {
        auto room =
            player->GetRoom();

        if(room)
        {
            room->Leave(player);
        }


        m_server.GetPlayerManager()
            .RemovePlayer(
                player->GetId()
            );
    }


    m_socket.close();
}