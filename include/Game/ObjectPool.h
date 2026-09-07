#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <concepts>

template<typename T>
concept PoolableObject = requires(T t) {
    { t.Reset() } -> std::same_as<void>;
};

template<typename T>
    requires PoolableObject<T>
class ObjectPool
{
private:
    std::vector<std::unique_ptr<T>> m_allInstances; // 메모리를 소유하는 벡터
    std::queue<T*> m_availablePool;                 // 대기 중인 빈 객체 포인터 큐

public:
    // 서버가 켜질 때 또는 방이 생성될 때 미리 할당 (Pre-allocation)
    void Initialize(size_t capacity)
    {
        m_allInstances.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i)
        {
            auto obj = std::make_unique<T>();
            m_availablePool.push(obj.get());
            m_allInstances.push_back(std::move(obj));
        }
    }

    // 객체 대여 (Spawn)
    template<typename... Args>
    T* Spawn(Args&&... args)
    {
        T* obj = nullptr;

        if (m_availablePool.empty())
        {
            // 풀이 모자라면 동적 확장
            auto newObj = std::make_unique<T>();
            obj = newObj.get();
            m_allInstances.push_back(std::move(newObj));
        }
        else
        {
            obj = m_availablePool.front();
            m_availablePool.pop();
        }

        // 객체 내부 초기화 함수 호출 (가변 인자 전달 가능)
        obj->Init(std::forward<Args>(args)...);
        return obj;
    }

    // 객체 반납 (Despawn)
    void Despawn(T* obj)
    {
        if (!obj) return;

        obj->Reset();
        m_availablePool.push(obj);
    }
};