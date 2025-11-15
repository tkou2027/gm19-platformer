#pragma once
#include "game_object.h"
#include "util/list.h"
#include "resource/stage_config.h"
#include "resource/tilemap_common.h"
#include "resource/tile.h"

namespace foo
{
	class TileMap;
	class Room : public GameObject
	{
	public:
		Room(const StageConfig& stage_config, int grid_id);
		void Initialize() override;
		void Update() override;
		void Finalize() override;
		void SetActive() override;
		void SetIdle() override;
		void SetRemoved() override;
		Rect GetBoundingBox();
		const Vector2& GetSavePosition() const { return m_save_position; }
		bool GetHasRain() const { return m_has_rain; }
	private:
		static constexpr int MAX_SPACE_BLOCKS{ 32 };
		static constexpr int MAX_BOUNDARY{ 4 };
		static constexpr int MAX_ITEMS{ 32 };
		void LoadStageConfig();
		void FillStageConfigTileMap(const GridConfig& grid_config, TileMapConfig& tile_map_config) const;
		void LoadStageConfigBoundary(int offset_index);
		void LoadStageConfigSpaceBlocks(SpaceId space_id); // TODO
		Vector2 GetTileOffset(int index, float decimal = 0.0f);
		//void LoadStageConfigSpaceBlocksRect(int mask);
		int m_scope_id{ -1 };
		int m_room_id{ -1 };
		int m_room_col{ -1 };
		int m_room_row{ -1 };
		Rect m_bbox{};
		const StageConfig& m_stage_config;
		const GridConfig& m_grid_config;
		// child objects
		TileMap* m_tile_map{ nullptr };
		List m_boundaries;
		List m_space_blocks; // list of space blocks
		List m_items; // list of ??
		Vector2 m_save_position{};
		bool m_has_rain{ false };
	};
}