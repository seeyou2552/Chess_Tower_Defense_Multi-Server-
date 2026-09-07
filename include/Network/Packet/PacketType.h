#pragma once
#include <cstdint>

enum class PacketType : uint16_t
{
    None = 0,

    LoginRequest = 1,
    LoginResponse = 2,

    RegisterRequest = 3,
    RegisterResponse = 4,

    JoinRoomRequest = 5,
    JoinRoomResponse = 6,

    StageStartResponse = 7,

    LoadSceneResponse = 8,
    LoadedScene = 9,

    // Minion
    SpawnMinionRequest = 10,
    SpawnMinionResponse = 11,
    SpawnMinionNotify = 12,

    RelocateMinionRequest = 13,
    RelocateMinionResponse = 14,
    RelocateMinionNotify = 15,

    SellMinionRequest = 27,
    SellMinionResponse = 28,

    UpgradeMinionRequest = 29,
    UpgradeMinionResponse = 30,
    UpgradeMinionNotify = 31,

    ReturnMinionNotify = 32,
    UpdateMinionNotify = 33,

    AttackNotify = 35,

    // Player
    UseGoldRequest = 16,
    UseGoldResponse = 17,
    UpdateGold = 25,

    // Wave
    StartWaveRequest = 18,
    StartWaveNotify = 19,
    EndWaveNotify = 23,

    // Enemy
    SpawnEnemyNotify = 20,
    ArrivalEnemyNotify = 21,
    DamageToEnemyNotify = 22,
    DeadEnemyNotify = 34,
    
    PingRequest,
    PingResponse
};