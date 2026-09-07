#include "Game/Minion/StatUpgradeManager.h"
#include "Game/Minion/StatUpgrade.h"

StatUpgradeManager& StatUpgradeManager::GetInstance()
{
    static StatUpgradeManager instance;
    return instance;
}

void StatUpgradeManager::Init()
{
    const int UPGRADE_ID_POWER = 2001;
    const int UPGRADE_ID_COOLDOWN = 2002;
    const int UPGRADE_ID_MAX_TARGETS = 2005;
    const int UPGRADE_ID_RANGE = 2004;
    const int UPGRADE_ID_MAX_AC = 2003;

    RegisterEvent(UPGRADE_ID_POWER, std::make_shared<PowerUpgrade>());
    RegisterEvent(UPGRADE_ID_COOLDOWN, std::make_shared<AtkCooldownUpgrade>());
    RegisterEvent(UPGRADE_ID_MAX_TARGETS, std::make_shared<SkillMaxTargetsUpgrade>());
    RegisterEvent(UPGRADE_ID_RANGE, std::make_shared<RangeUpgrade>());
    RegisterEvent(UPGRADE_ID_MAX_AC, std::make_shared<MaxACUpgrade>());
}