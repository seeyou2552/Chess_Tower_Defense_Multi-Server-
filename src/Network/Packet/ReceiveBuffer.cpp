#include "Network/Packet/ReceiveBuffer.h"

#include <algorithm>

void ReceiveBuffer::Append(
    const char* data,
    size_t size
)
{
    m_buffer.insert(
        m_buffer.end(),
        data,
        data + size
    );
}

const char* ReceiveBuffer::Data() const
{
    return m_buffer.data();
}

size_t ReceiveBuffer::Size() const
{
    return m_buffer.size();
}

void ReceiveBuffer::Remove(size_t size)
{
    if(size >= m_buffer.size())
    {
        m_buffer.clear();
        return;
    }

    m_buffer.erase(
        m_buffer.begin(),
        m_buffer.begin() + size
    );
}