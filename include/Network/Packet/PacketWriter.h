#pragma once

#include <vector>
#include <string>
#include <cstring>

#include "Network/Packet/PacketHeader.h"
#include "Network/Packet/PacketType.h"

class PacketWriter
{
public:

    explicit PacketWriter(
        PacketType type
    );

    template<typename T>
    void Write(const T& value)
    {
        Write(
            &value,
            sizeof(T)
        );
    }

    void Write(
        const void* data,
        size_t size
    );

    void WriteString(
        const std::string& value
    );

    void Finish();

    const std::vector<char>& GetBuffer() const;

private:

    PacketType m_type;

    std::vector<char> m_buffer;
};