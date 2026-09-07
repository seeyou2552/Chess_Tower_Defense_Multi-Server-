#include "Game/Minion/StatUpgrade.h"
#include "Game/Minion/Minion.h"

void PowerUpgrade::OnUpgrade(std::shared_ptr<Minion>& minion, float value) {
    minion->AddBasePower(value);
}

void AtkCooldownUpgrade::OnUpgrade(std::shared_ptr<Minion>& minion, float value) {
    minion->AddBaseAttackCooldown(value);
}

void SkillMaxTargetsUpgrade::OnUpgrade(std::shared_ptr<Minion>& minion, float value) {
    minion->AddBaseSkillMaxTargets(value);
}

void RangeUpgrade::OnUpgrade(std::shared_ptr<Minion>& minion, float value) {
    minion->AddBaseRange(value / 2.0f);
}

void MaxACUpgrade::OnUpgrade(std::shared_ptr<Minion>& minion, float value) {
    minion->AddBaseMaxAC(value);
}