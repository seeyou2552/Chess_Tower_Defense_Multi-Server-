#pragma once

#include <memory>

#include "Game/GameState.h"
#include "Game/WaveManager.h"

class Room;
class EnemyDataManager;
class StageData;

class GameInstance
{
public:

    explicit GameInstance(
        std::shared_ptr<Room> room
    );

    void Start();

    void Update( float deltaTime );

    void Stop();

	uint32_t GetStageId() const
	{
		return m_stageId;
	}

private:

    std::weak_ptr<Room> m_room;

    bool m_running = false;

    uint32_t m_stageId;
};