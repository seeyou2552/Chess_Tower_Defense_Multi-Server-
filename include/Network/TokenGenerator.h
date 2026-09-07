#pragma once

#include <string>

class TokenGenerator
{
public:
    static std::string Generate(std::size_t byteLength = 32);
};