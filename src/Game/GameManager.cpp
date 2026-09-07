#include "Game/GameManager.h"

#include "Game/GameInstance.h"
#include "Game/Room.h"

#include "Game/TickManager.h"

#include "Game/Enemy/EnemyDataManager.h"

GameManager::GameManager(
    TickManager& tickManager
)
    :
    m_tickManager(
        tickManager
    )
{

}

std::shared_ptr<GameInstance>
GameManager::CreateGame(
    std::shared_ptr<Room> room
)
{
    auto game =
        std::make_shared<GameInstance>(
            room
        );

    m_games.emplace(
        room->GetId(),
        game
    );

    room->SetGame(
        game
    );

    m_tickManager.Add(
        [game]
        (
            float dt
        )
        {
            game->Update(dt);
        }
    );

    game->Start();

    return game;
}

std::shared_ptr<GameInstance>
GameManager::GetGame(
    uint32_t roomId
)
{
    auto iter =
        m_games.find(
            roomId
        );

    if(iter == m_games.end())
        return nullptr;

    return iter->second;
}

void GameManager::RemoveGame(
    uint32_t roomId
)
{
    m_games.erase(
        roomId
    );
}