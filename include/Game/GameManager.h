#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>

class Room;
class GameInstance;
class TickManager;
class EnemyDataManager;

class GameManager
{
public:

    GameManager(
        TickManager& tickManager
    );

    std::shared_ptr<GameInstance>
    CreateGame(
        std::shared_ptr<Room> room
    );

    std::shared_ptr<GameInstance>
    GetGame(
        uint32_t roomId
    );

    void RemoveGame(
        uint32_t roomId
    );

private:

    TickManager& m_tickManager;

    std::unordered_map<
        uint32_t,
        std::shared_ptr<GameInstance>
    > m_games;
};