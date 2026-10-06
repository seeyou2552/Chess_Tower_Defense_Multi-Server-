#pragma once

#include <queue>
#include <memory>
#include <functional>
#include <cstddef>
#include <mutex>
#include <utility>

template<typename T>
class ObjectPool
{
public:

    explicit ObjectPool(
        std::function<std::shared_ptr<T>()> creator
    )
        :
        m_creator(creator)
    {
    }

    std::shared_ptr<T> Acquire()
    {
        std::shared_ptr<T> obj;
        bool needsReset = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            const std::size_t pendingCount = m_pending.size();
            for (std::size_t i = 0; i < pendingCount; ++i)
            {
                auto pending = std::move(m_pending.front());
                m_pending.pop();

                if (!obj && pending.use_count() == 1)
                {
                    obj = std::move(pending);
                    needsReset = true;
                }
                else
                {
                    m_pending.push(std::move(pending));
                }
            }
        }

        if (!obj)
            return m_creator();

        if (needsReset)
            obj->Reset();

        return obj;
    }

    void Release(
        std::shared_ptr<T> obj
    )
    {
        if (!obj)
            return;

        std::lock_guard<std::mutex> lock(m_mutex);
        m_pending.push(std::move(obj));
    }

private:

    std::mutex m_mutex;

    std::queue<
        std::shared_ptr<T>
    > m_pending;

    std::function<
        std::shared_ptr<T>()
    > m_creator;
};