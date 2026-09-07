#include "Game/PlayerManager.h"
#include "Game/Player.h"

#include "Network/TokenGenerator.h"
#include "Core/Logger.h"

std::shared_ptr<Player>
PlayerManager::CreatePlayer()
{
    // 임의의 토큰 생성
    auto token = TokenGenerator::Generate();

    auto player = std::make_shared<Player>(
        m_nextId,
        token
    );

    m_players.emplace(
        m_nextId,
        player
    );

    m_nextId++;

    return player;
}



void PlayerManager::RemovePlayer(
    uint32_t id
)
{
    m_players.erase(id);

    Logger::GetInstance().Info(
        "Player Removed : "
        + std::to_string(id)
    );

    Logger::GetInstance().Info(
        "Player Count : "
        + std::to_string(m_players.size())
    );
}

std::shared_ptr<Player>
PlayerManager::GetPlayer(
    uint32_t id
)
{
    auto iter =
        m_players.find(id);

    if(iter == m_players.end())
        return nullptr;

    return iter->second;
}