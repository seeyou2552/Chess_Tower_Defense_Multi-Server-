#include "Game/Skill/SkillManager.h"
#include "Game/Skill/BuffSKill.h"

SkillManager& SkillManager::GetInstance()
{
    static SkillManager instance;
    return instance;
}

void SkillManager::Init()
{
    const int BUFF_ID_ATK_SPEED = 4001;
    const int BUFF_ID_MAX_TARGETS = 4002;
    const int BUFF_ID_POWER = 4003;

    RegisterBuffSkill(BUFF_ID_POWER, std::make_shared<PowerUpBuff>());
    RegisterBuffSkill(BUFF_ID_ATK_SPEED, std::make_shared<AtkSpeedBuff>());
}