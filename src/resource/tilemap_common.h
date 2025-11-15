#pragma once
#include "math/vector2.h"
#include "math/rect.h"
#include "tile.h"
#include "space_config.h"

namespace foo
{
	struct TileMapInterval
	{
		int row_start{ 0 };
		int col_start{ 0 };
		int row_end{ 0 };
		int col_end{ 0 };
	};
	class TileMapConfig
	{
	public:
		// data
		int cols{ 0 };
		int rows{ 0 };
		Vector2 offset{};
		Vector2 tile_size{};
		const char* tile_chars{ nullptr }; // save space
		TileType* tile_types{ nullptr };
		Tile* tiles{ nullptr };
		SpaceId space_id{ SpaceId::SPACE_DEFAULT };
		
		// helpers
		void SetCenterOffset()
		{
			const Vector2 size{ cols * tile_size.x, rows * tile_size.y };
			offset = (tile_size - size) * 0.5f;
		}
		TileType GetTileTypeAt(int row, int col) const
		{
			// TODO
			char tile_char = tile_chars[row * cols + col];
			switch (space_id)
			{
			case SpaceId::SPACE_DEFAULT:
				return tile_char & 1 ? TileType::SOLID : TileType::NONE;
			case SpaceId::SPACE_A:
				return tile_char & 2 ? TileType::SOLID_SPACE : TileType::NONE;
			case SpaceId::SPACE_B:
				return tile_char & 4 ? TileType::SOLID_SPACE : TileType::NONE;
			}
			return TileType::NONE;
		}
		const Tile* GetTileAt(int row, int col) const
		{
			// static, no state
			if (!tiles)
			{
				return nullptr;
			}
			const Tile* tile = tiles + row * cols + col;
			return tile;
		}
		const Rect GetBoundingBox(const Vector2& position) const
		{
			const Vector2 size{ cols * tile_size.x, rows * tile_size.y };
			return Rect{ size, position };
		}
		bool IfOverlap(const Rect& other, const Vector2& position, TileMapInterval& interval) const
		{
			Rect my{ GetBoundingBox(position) };
			if (!other.IfOverlap(my))
			{
				return false;
			}

			const Vector2 other_top_left{ other.GetTopLeft() };
			const Vector2 other_size{ other.GetSize() };
			const Vector2 my_top_left{ my.GetTopLeft() };
			interval.col_start = Math::Max(0, (int)floorf((other_top_left.x - my_top_left.x) / tile_size.x));
			interval.col_end = Math::Min(cols, (int)ceilf((other_top_left.x + other_size.x - my_top_left.x) / tile_size.x));
			interval.row_start = Math::Max(0, (int)floorf((other_top_left.y - my_top_left.y) / tile_size.y));
			interval.row_end = Math::Min(rows, (int)ceilf((other_top_left.y + other_size.y - my_top_left.y) / tile_size.y));
			return true;
		}
	};
}