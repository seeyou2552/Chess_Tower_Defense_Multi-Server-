#pragma once

#include <memory>
#include <cstdint>

class Session;

class RegisterHandler
{

public:

    void Handle(
        std::shared_ptr<Session> session,
        const char* packet,
        uint16_t size
    );

};