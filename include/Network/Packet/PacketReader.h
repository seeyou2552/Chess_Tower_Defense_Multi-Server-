#pragma once

#include <cstring>
#include <string>
#include <cstdint>
#include <stdexcept>

class PacketReader
{
public:

    PacketReader(
        const char* data,
        size_t size
    )
        :
        m_data(data),
        m_size(size),
        m_offset(0)
    {
    }

    template<typename T>
    void Read(T& value)
    {
        if(m_offset + sizeof(T) > m_size)
        {
            throw std::runtime_error("PacketReader Overflow");
        }

        memcpy(
            &value,
            m_data + m_offset,
            sizeof(T)
        );

        m_offset += sizeof(T);
    }

    void ReadString(
        std::string& value
    )
    {
        uint16_t length;

        Read(length);

        if(m_offset + length > m_size)
        {
            throw std::runtime_error("PacketReader Overflow");
        }

        value.assign(
            m_data + m_offset,
            length
        );

        m_offset += length;
    }

private:

    const char* m_data;

    size_t m_size;

    size_t m_offset;
};