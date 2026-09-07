#pragma once

#include <string>

class SHA256
{
public:
    static std::string Hash(
        const std::string& text
    );

    static std::string GenerateSalt(
        size_t length = 16
    );
};