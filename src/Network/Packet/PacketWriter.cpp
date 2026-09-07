#include "Network/Packet/PacketWriter.h"

PacketWriter::PacketWriter(PacketType type)
    : m_type(type)
{
    m_buffer.resize(sizeof(PacketHeader));
}

void PacketWriter::Write(
    const void* data,
    size_t size
)
{
    auto oldSize = m_buffer.size();

    m_buffer.resize(oldSize + size);

    memcpy(
        m_buffer.data() + oldSize,
        data,
        size
    );
}

void PacketWriter::WriteString(
    const std::string& value
)
{
    uint16_t length =
        static_cast<uint16_t>(value.size());

    Write(length);

    Write(
        value.data(),
        length
    );
}

void PacketWriter::Finish()
{
    auto* header =
        reinterpret_cast<PacketHeader*>(
            m_buffer.data()
        );

    header->size =
        static_cast<uint16_t>(m_buffer.size());

    header->type = m_type;
}

const std::vector<char>& PacketWriter::GetBuffer() const
{
    return m_buffer;
}