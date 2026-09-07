#pragma once

#include <memory>

class Minion;
class Enemy;
class Room;

struct KillEventData {
    int id;
    float value;
    float duration;
};

class IKillEvent {
public:
    virtual ~IKillEvent() = default;
    virtual void OnKill(const std::shared_ptr<Minion>& minion, const KillEventData& data, const std::shared_ptr<Room>& room) = 0;
};

class GoldOnKillEvent : public IKillEvent {
public:
    void OnKill(const std::shared_ptr<Minion>& minion, const KillEventData& data, const std::shared_ptr<Room>& room) override;
};

class PowerUpKillEvent : public IKillEvent {
public:
    void OnKill(const std::shared_ptr<Minion>& minion, const KillEventData& data, const std::shared_ptr<Room>& room) override;
};