#include "Game/RoomManager.h"
#include "Game/Room.h"
#include "Game/TickManager.h"
#include "Game/Stage/StageData.h"
#include "Game/Stage/StageManager.h"
#include "Game/WaveManager.h"

#include "Core/Logger.h"

#include <algorithm>
#include <chrono>
#include <future>
#include <thread>

RoomManager::RoomManager(
    TickManager& tickManager,
    StageManager& stageManager,
    SpawnManager& spawnManager
)
    :
    m_tickManager(tickManager),
    m_stageManager(stageManager),
    m_spawnManager(spawnManager),
    m_threadPool(
        std::max<std::size_t>(
            1,
            std::thread::hardware_concurrency() > 1
                ? std::thread::hardware_concurrency() - 1
                : 1
        )
    )
{
    m_tickManager.Add(
        [this](float deltaTime)
        {
            UpdateRooms(deltaTime);
        }
    );
}

std::shared_ptr<Room>
RoomManager::CreateRoom( uint32_t stageId )
{
    auto stageData =
        m_stageManager.GetStage(stageId);

    auto tileData = 
        m_stageManager.GetStageTile(stageId);

    uint32_t roomId;

    {
        std::lock_guard<std::mutex> lock(m_roomMutex);
        roomId = m_nextRoomId++;
    }

    auto room =
        std::make_shared<Room>(
            roomId,
            m_tickManager,
            m_spawnManager,
            stageData,
            tileData
        );

    {
        std::lock_guard<std::mutex> lock(m_roomMutex);
        m_rooms.emplace(roomId, room);
    }

    Logger::GetInstance().Info(std::to_string(roomId));

    return room;
}

std::shared_ptr<Room>
RoomManager::GetRoom(
    uint32_t stageId
)
{
    std::vector<std::shared_ptr<Room>> rooms;

    {
        std::lock_guard<std::mutex> lock(m_roomMutex);
        rooms.reserve(m_rooms.size());
        for (const auto& [roomId, room] : m_rooms)
        {
            if (room)
            {
                rooms.push_back(room);
            }
        }
    }

    for (const auto& room : rooms)
    {
        if (room->GetStageId() == stageId)
        {
            if (!room->IsFull())
            {
                return room;
            }

            Logger::GetInstance().Info("IsFull");
        }
    }
    Logger::GetInstance().Info("Room not found");
    return nullptr;
}

void RoomManager::RemoveRoom(
    uint32_t roomId
)
{
    std::lock_guard<std::mutex> lock(m_roomMutex);
    m_rooms.erase(roomId);
}

void RoomManager::UpdateRooms(float deltaTime)
{
    const auto updateStart = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<Room>> rooms;

    {
        std::lock_guard<std::mutex> lock(m_roomMutex);
        rooms.reserve(m_rooms.size());

        for (const auto& [roomId, room] : m_rooms)
        {
            if (room)
            {
                rooms.push_back(room);
            }
        }
    }

    std::vector<std::future<void>> updates;
    updates.reserve(rooms.size());

    for (const auto& room : rooms)
    {
        updates.push_back(
            m_threadPool.Enqueue(
                [room, deltaTime]
                {
                    room->Update(deltaTime);
                }
            )
        );
    }

    for (auto& update : updates)
    {
        update.get();
    }

    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - updateStart
    ).count();

    Logger::GetInstance().Info(
        "Parallel Room Update: " + std::to_string(elapsed) + " us, rooms="
        + std::to_string(rooms.size())
    );
}