#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Game/Stage/StageData.h"
#include "Game/Stage/StageTileData.h"

class StageManager
{
public:

    bool Load(const std::string& file);

    std::shared_ptr<StageData> GetStage(
        int stageId
    );

	std::shared_ptr<StageTileData> GetStageTile(
		int stageId
	);

    bool LoadAllStageTiles();

	uint32_t ExtractStageIdFromFilename(
		const std::string& filename
	);

private:

    std::unordered_map<
        int,
        std::shared_ptr<StageData>
    > m_stages;

    std::unordered_map<uint32_t, StageTileData> m_allStageMaps;
};