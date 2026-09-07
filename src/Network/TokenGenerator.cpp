#include "Network/TokenGenerator.h"

#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

std::string TokenGenerator::Generate(std::size_t byteLength)
{
    std::vector<unsigned char> buffer(byteLength);

    if (RAND_bytes(buffer.data(), static_cast<int>(buffer.size())) != 1)
    {
        throw std::runtime_error("RAND_bytes failed.");
    }

    std::ostringstream oss;

    for (unsigned char b : buffer)
    {
        oss << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(b);
    }

    return oss.str();
}