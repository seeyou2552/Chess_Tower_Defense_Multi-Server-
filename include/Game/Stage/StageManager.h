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
        uint32_t stageId
    );

	std::shared_ptr<StageTileData> GetStageTile(
		uint32_t stageId
	);

    bool LoadAllStageTiles();

	uint32_t ExtractStageIdFromFilename(
		const std::string& filename
	);

private:

    std::unordered_map<
        uint32_t,
        std::shared_ptr<StageData>
    > m_stages;

    std::unordered_map<uint32_t, StageTileData> m_allStageMaps;
};