#include "Game/TickManager.h"

#include "Game/Math/Timestamp.h"

void TickManager::Add(TickFunc func)
{
    // 멀티스레드 환경이라면 여기서 락(Lock)을 걸어주거나, 
    // 혹은 Update 도중이라면 대기열(Pending)에 넣고 Update가 끝난 뒤에 일괄 추가해야 안전합니다.
    m_ticks.push_back(func);
}

void TickManager::Update(float deltaTime)
{
    m_timestamp = GetCurrentTimestampMs();

    // 복사해서 쓰거나, 순회 도중 추가/삭제에 안전한 구조로 설계해야 합니다.
    // 임시 복사본을 만들어 순회하는 방법 (단, 성능에 미치는 영향 고려 필요)
    auto currentTicks = m_ticks;
    for (auto& tick : currentTicks)
    {
        if (tick)
        {
            tick(deltaTime);
        }
    }
}