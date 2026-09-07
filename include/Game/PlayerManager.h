#pragma once

#include <unordered_map>
#include <memory>

class Player;

class PlayerManager
{

public:

    std::shared_ptr<Player>
    CreatePlayer();

    void RemovePlayer(
        uint32_t id
    );

    std::shared_ptr<Player>
    GetPlayer(
        uint32_t id
    );

private:

    uint32_t m_nextId = 1;

    std::unordered_map
    <
        uint32_t,
        std::shared_ptr<Player>
    >
    m_players;
};