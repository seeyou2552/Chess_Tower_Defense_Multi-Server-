#include "Game/Skill/KillEvent.h"

#include "Game/Minion/Minion.h"
#include "Game/Enemy/Enemy.h"
#include "Game/Room.h"

void GoldOnKillEvent::OnKill(const std::shared_ptr<Minion>& minion, const KillEventData& data, const std::shared_ptr<Room>& room)
{
    // 플레이어에 골드 추가
    room->PlayerAddGold(static_cast<int>(data.value));
}


void PowerUpKillEvent::OnKill(const std::shared_ptr<Minion>&, const KillEventData& data, const std::shared_ptr<Room>& room)
{

}
