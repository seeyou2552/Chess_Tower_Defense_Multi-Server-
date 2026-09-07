#pragma once

#include <string>
#include <memory>
#include <mutex>

#include "Game/Room.h"

class Session;
class Room;
class Account;

class Player
{

public:

    explicit Player(
        uint32_t id,
        const std::string& token
    );

    void Update(float deltaTime);

public:

    uint32_t GetId() const
    {
        return m_id;
    }

	const std::string& GetToken() const
	{
		return m_token;
	}

    void SetAccount(
        std::shared_ptr<Account> account
    )
    {
        m_account = account;
    }

    void SetSession(std::shared_ptr<Session> session)
    {
        m_session = session;
    }

    std::shared_ptr<Session> GetSession()
    {
        return m_session.lock();
    }

    void SetRoom(std::shared_ptr<Room> room)
    {
        m_room = room;
    }

    std::shared_ptr<Room> GetRoom() const
    {
        return m_room.lock();
    }

    int GetGold() const
    {
        return m_gold;
    }

    void AddGold(int amount)
    {
        m_gold += amount;
    }

    bool TryUseGold(int price);

private:

    std::mutex m_playerMutex;

    uint32_t m_id;
    std::string m_token;

    std::weak_ptr<Session> m_session;
    std::weak_ptr<Room> m_room;
    std::weak_ptr<Account> m_account;

    std::atomic<int> m_gold;

};