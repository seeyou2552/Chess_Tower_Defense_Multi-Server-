#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>
#include <mutex>
#include <vector>

#include "Core/ThreadPool.h"

class Room;
class TickManager;
class StageManager;
class SpawnManager;
class WaveManager;

class RoomManager
{
public:

    RoomManager(
        TickManager& tickManager,
        StageManager& stageManager,
        SpawnManager& spawnManager
    );

    std::shared_ptr<Room> CreateRoom(
		uint32_t stageId
    );

    std::shared_ptr<Room> GetRoom(
        uint32_t stageId
    );

    void RemoveRoom(
        uint32_t roomId
    );

    void UpdateRooms(float deltaTime);

private:

    uint32_t m_nextRoomId = 1;

    std::unordered_map<
        uint32_t,
        std::shared_ptr<Room>
    > m_rooms;

    mutable std::mutex m_roomMutex;

    TickManager& m_tickManager;
    StageManager& m_stageManager;
    SpawnManager& m_spawnManager;

    ThreadPool m_threadPool;
};