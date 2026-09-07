#pragma once

#include <memory>

class Minion;

struct UpgradeSlot {
    int currentLevel = 0;
    int maxLevel;
    int price;
    float amount;
};

class IStatUpgrade {
public:
    virtual ~IStatUpgrade() = default;
    virtual void OnUpgrade(std::shared_ptr<Minion>&, float value) = 0;
};

class PowerUpgrade : public IStatUpgrade {
public:
    void OnUpgrade(std::shared_ptr<Minion>&, float value) override;
};

class AtkCooldownUpgrade : public IStatUpgrade {
public:
    void OnUpgrade(std::shared_ptr<Minion>&, float value) override;
};

class SkillMaxTargetsUpgrade : public IStatUpgrade {
public:
    void OnUpgrade(std::shared_ptr<Minion>&, float value) override;
};

class RangeUpgrade : public IStatUpgrade {
public:
    void OnUpgrade(std::shared_ptr<Minion>&, float value) override;

};

class MaxACUpgrade : public IStatUpgrade {
public:
    void OnUpgrade(std::shared_ptr<Minion>&, float value) override;
};