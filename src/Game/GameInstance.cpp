#include "Game/GameInstance.h"
#include "Game/Stage/StageData.h"
#include "Game/Room.h"
#include "Network/Packet/StagePacket.h"
#include "Network/Packet/LoadScenePacket.h"
#include "Network/Packet/PacketWriter.h"

#include "Core/Logger.h"


GameInstance::GameInstance(
    std::shared_ptr<Room> room
)
    :
    m_room(room)
{

}


void GameInstance::Start()
{
    m_running = true;


    Logger::GetInstance()
        .Info(
            "Game Start"
        );

    auto room = m_room.lock();

    if (!room)
    {
        return;
    }

    // 이후 room 내 유저에게 StageStrat 패킷을 전송
    room->StageStart();
}

void GameInstance::Update( float deltaTime )
{
    if(!m_running)
        return;


    auto room =
        m_room.lock();


    if (!room)
        return;
}

void GameInstance::Stop()
{
    m_running = false;


    Logger::GetInstance()
        .Info(
            "Game Stop"
        );
}