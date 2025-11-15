#pragma once
#include "math/vector2.h"
#include "math/rect.h"

namespace foo
{
	struct RainConfig
	{
		int width{ 0 };
		int height{ 0 };
		int index{ 0 };
	};

	struct GridConfig
	{
		static constexpr int TILE_COLS{ 40 }; // 32
		static constexpr int TILE_ROWS{ 23 }; // 18
		static constexpr int TILE_CNT{ TILE_COLS * TILE_ROWS };

		static constexpr int MAX_RAIN{ 1 };
		static constexpr int MAX_FAN{ 4 };
		static constexpr int MAX_ENEMY{ 16 };


		char* tiles{ nullptr };//[TILE_CNT + 1];
		int scope_id{ -1 };
		int save_point{ 0 };
		// items
		float fan_point[MAX_FAN]{};
		int enemy_point[MAX_ENEMY]{};
		// rain
		int rain_cnt{ 0 };
		RainConfig rain_config[MAX_RAIN];
		// item
		char item{ '\0' };
		int item_index{ 0 };
	};
	struct StageConfig
	{
		static constexpr int GRID_COLS{ 9 };
		static constexpr int GRID_ROWS{ 2 };
		static constexpr int GRID_CNT{ GRID_COLS * GRID_ROWS };
		static const Vector2 TILE_SIZE;
		static const Vector2 GRID_SIZE;

		GridConfig grids[GRID_CNT];
	};

	extern StageConfig g_stage_config;
}