#include "Game/Player.h"

#include <string>

#include "Core/Logger.h"

Player::Player(uint32_t id, const std::string& token)
    : 
    m_id(id),          // 기존 플레이어 고유 ID 초기화
    m_token(token)     // 전달받은 토큰 초기화
{
    m_gold = 400; // 시작 시 골드 (임의)
}

bool Player::TryUseGold(int price)
{
    if (m_gold < price)
        return false;

    m_gold -= price;
    return true;
}