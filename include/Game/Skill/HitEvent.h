#pragma once

#include <memory>

class Minion;
class Enemy;
class Room;

struct HitEventData {
    int id;
    float value;
    float duration;
};

class IHitEvent {
public:
    virtual ~IHitEvent() = default;
    virtual void OnHit(
        const std::shared_ptr<Minion>& minion,
        const std::shared_ptr<Enemy>& enemy,
        const HitEventData& data,
        const std::shared_ptr<Room>& room
    ) = 0;
};

class SlowOnHitEvent : public IHitEvent {
public:
    void OnHit(
        const std::shared_ptr<Minion>& minion,
        const std::shared_ptr<Enemy>& enemy,
        const HitEventData& data,
        const std::shared_ptr<Room>& room
    ) override;
};

class StunOnHitEvent : public IHitEvent {
public:
    void OnHit(
        const std::shared_ptr<Minion>& minion,
        const std::shared_ptr<Enemy>& enemy,
        const HitEventData& data,
        const std::shared_ptr<Room>& room
    ) override;
};

class DamageOverTimeOnHitEvent : public IHitEvent {
public:
    void OnHit(
        const std::shared_ptr<Minion>& minion,
        const std::shared_ptr<Enemy>& enemy,
        const HitEventData& data,
        const std::shared_ptr<Room>& room
    ) override;
};