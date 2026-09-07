#include "Game/Skill/BuffSkill.h"
#include "Game/Minion/Minion.h"

#include <memory>

void PowerUpBuff::OnBuff(std::vector<std::shared_ptr<Minion>>& minions, const BuffData& data) const
{
    for (auto& minion : minions)
    {
        minion->AddPowerBuff(data);

        if (data.changeAttackSpec)
            minion->AddAttackSpecBuff(*data.changeAttackSpec, data.duration);
    }
}

void AtkSpeedBuff::OnBuff(std::vector<std::shared_ptr<Minion>>& minions, const BuffData& data) const
{
    for (auto& minion : minions)
    {
        minion->AddAttackSpeedBuff(data);
    }
}