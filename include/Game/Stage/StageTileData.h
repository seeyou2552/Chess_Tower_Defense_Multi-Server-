#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Game/Math/Vector3.h"

// 스테이지별 맵 데이터를 담는 클래스/구조체
struct StageTileData
{
    int stageId;

    // 설치 가능한 타일들의 좌표 목록을 O(1) 탐색이 가능한 셋(Set)으로 관리
    std::unordered_set<Vector3Int, Vector3IntHash> buildableTiles;
};