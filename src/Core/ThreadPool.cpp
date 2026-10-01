#include "Core/ThreadPool.h"

#include <stdexcept>

ThreadPool::ThreadPool(std::size_t threadCount)
{
    if (threadCount == 0)
    {
        threadCount = 1;
    }

    m_workers.reserve(threadCount);

    for (std::size_t index = 0; index < threadCount; ++index)
    {
        m_workers.emplace_back(&ThreadPool::WorkerLoop, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
    }

    m_condition.notify_all();

    for (auto& worker : m_workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

std::future<void> ThreadPool::Enqueue(std::function<void()> task)
{
    std::packaged_task<void()> packagedTask(std::move(task));
    auto future = packagedTask.get_future();

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_stop)
        {
            throw std::runtime_error("ThreadPool is stopping");
        }

        m_tasks.emplace(std::move(packagedTask));
    }

    m_condition.notify_one();
    return future;
}

void ThreadPool::WorkerLoop()
{
    while (true)
    {
        std::packaged_task<void()> task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(
                lock,
                [this]
                {
                    return m_stop || !m_tasks.empty();
                }
            );

            if (m_stop && m_tasks.empty())
            {
                return;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }

        task();
    }
}