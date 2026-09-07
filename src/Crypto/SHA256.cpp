#include "Crypto/SHA256.h"

#include <openssl/sha.h>
#include <random>
#include <sstream>
#include <iomanip>


std::string SHA256::Hash(
    const std::string& text
)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(
        &sha256,
        text.c_str(),
        text.size()
    );
    SHA256_Final(
        hash,
        &sha256
    );


    std::stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << (int)hash[i];
    }

    return ss.str();
}


std::string SHA256::GenerateSalt(
    uint64_t length
)
{
    static const char chars[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";


    std::random_device rd;

    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(
        0,
        sizeof(chars) - 2
    );


    std::string salt;

    for (uint64_t i = 0; i < length; i++)
    {
        salt += chars[dis(gen)];
    }

    return salt;
}