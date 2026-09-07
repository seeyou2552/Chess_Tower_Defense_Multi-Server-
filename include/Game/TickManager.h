#pragma once

#include <functional>
#include <vector>
#include <mutex>

#include "Game/Math/Timestamp.h"


class TickManager
{
public:

    using TickFunc =
        std::function<void(float)>;


public:

    void Add(
        TickFunc func
    );


    void Update(
        float deltaTime
    );


    uint64_t GetTimestamp() const
    {
        return m_timestamp;
    }


private:

    std::vector<TickFunc> m_ticks;
    uint64_t m_timestamp = 0;
};