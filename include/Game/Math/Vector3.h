#pragma once

// 2차원/3차원 좌표를 표현하기 위한 구조체 (해시 연산자 오버로딩 필요)
struct Vector3Int
{
    int x, y, z;

    // 해시 맵에서 키로 쓰기 위해 비교 연산자 정의
    bool operator==(const Vector3Int& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct Vector3
{
    float x;
    float y;
    float z;
};

// Vector3Int를 unordered_set에서 쓰기 위한 커스텀 해시 함수
struct Vector3IntHash {
    size_t operator()(const Vector3Int& pos) const noexcept
    {
        size_t h1 = std::hash<int>{}(pos.x);
        size_t h2 = std::hash<int>{}(pos.y);
        size_t h3 = std::hash<int>{}(pos.z);

        size_t result = h1;

        result ^= h2 + 0x9e3779b9 + (result << 6) + (result >> 2);
        result ^= h3 + 0x9e3779b9 + (result << 6) + (result >> 2);

        return result;
    }
};