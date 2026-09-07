#pragma once

#include <vector>
#include <cstddef>

class ReceiveBuffer
{
public:

    void Append(
        const char* data,
        size_t size
    );

    const char* Data() const;

    size_t Size() const;

    void Remove(size_t size);

private:

    std::vector<char> m_buffer;
};