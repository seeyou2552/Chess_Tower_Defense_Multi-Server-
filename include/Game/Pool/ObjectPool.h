#pragma once

#include <queue>
#include <memory>
#include <functional>

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
        if (m_pool.empty())
        {
            return m_creator();
        }

        auto obj = m_pool.front();

        m_pool.pop();

        return obj;
    }

    void Release(
        std::shared_ptr<T> obj
    )
    {
        obj->Reset();

        m_pool.push(obj);
    }

private:

    std::queue<
        std::shared_ptr<T>
    > m_pool;

    std::function<
        std::shared_ptr<T>()
    > m_creator;
};