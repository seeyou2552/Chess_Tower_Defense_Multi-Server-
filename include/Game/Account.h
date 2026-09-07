#pragma once

#include <string>
#include <cstdint>


struct Account
{
    uint32_t id;

    std::string uuid;

    std::string loginId;

    std::string password;

    std::string salt;
};