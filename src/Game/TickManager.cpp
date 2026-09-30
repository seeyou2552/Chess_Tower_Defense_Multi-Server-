#include "Game/TickManager.h"

#include "Game/Math/Timestamp.h"

#include "Core/Logger.h"

void TickManager::Add(TickFunc func)
{
    Logger::GetInstance().Info("Add 1");

    std::lock_guard<std::mutex> lock(m_mutex);

    Logger::GetInstance().Info("Add 2");

    m_ticks.push_back(std::move(func));

    Logger::GetInstance().Info("Add 3");
}

void TickManager::Update(float deltaTime)
{
    m_timestamp = GetCurrentTimestampMs();

    std::vector<std::function<void(float)>> ticks;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ticks = m_ticks;
    }

    for (auto& tick : ticks)
    {
        tick(deltaTime);
    }

}