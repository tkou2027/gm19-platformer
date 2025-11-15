#include "render_tilemap.h"

#include "object/tilemap.h"
#include "global_context.h"
#include "resource/tile.h"
#include "math/rect.h"
#include "render/camera.h"

namespace foo
{
	RenderTileMap::RenderTileMap(TileMapConfig& tile_map_config)
		: m_tile_map_config(tile_map_config) {
	}
	void RenderTileMap::UpdateRenderData()
	{
		TileMapInterval visible_interval{};
		if (!IfInsideView(visible_interval))
		{
			return;
		}

		RenderSystem* render_system = g_global_context.m_render_system;
		TileManager* tile_manager = g_global_context.m_tile_manager;

		// Tile* tile = m_tile_map_config.tiles;
		// for (int row = 0; row < m_tile_map_config.rows; ++row)
		// for (int col = 0; col < m_tile_map_config.cols; ++col)
		for (int row = visible_interval.row_start; row < visible_interval.row_end; ++row)
		{
			for (int col = visible_interval.col_start; col < visible_interval.col_end; ++col)
			{
				// Tile* tile = m_tile_map_config.tiles + row * m_tile_map_config.cols + col;
				// bool tile = m_tile_map_config.tile_chars + row * m_tile_map_config.cols + col;
				TileType tile_type = m_tile_map_config.GetTileTypeAt(row, col);
				if (tile_type != TileType::NONE)
				{
					const TileView& tile_view = tile_manager->GetTileView(tile_type);
					SpriteConfigOutput sprite_config{ tile_view.sprite_config_output };
					sprite_config.offset = Vector2{
						col * m_tile_map_config.tile_size.x + m_tile_map_config.offset.x,
						row * m_tile_map_config.tile_size.y + m_tile_map_config.offset.y
					};
					sprite_config.size = m_tile_map_config.tile_size;
					SpaceId space_id = m_tile_map_config.space_id;

					// TODO more states
					const Tile* tile = m_tile_map_config.GetTileAt(row, col);
					{
						if (tile && !tile->active)
						{
							continue;
						}
					}
					//if (!tile->active)
					//{
					//	continue;
					//	// space_id = SpaceId::SPACE_DEFAULT;
					//}
					// sprite_config.color = g_space_config.GetSpaceProp(tile->space_id).color;
					Matrix4x4 mat = m_object->GetTransform().GetMatrix();
					render_system->GetTileMapData().Add(sprite_config, space_id, m_object->GetTransform().GetMatrix());
				}
			}
		}
	}
	bool RenderTileMap::IfInsideView(TileMapInterval& interval) const
	{
		const Rect& camera_rect = g_global_context.m_render_system->GetCamera().GetBoundingBox();
		Transform2D trans = m_object->GetTransform().GetTransform2D();
		return m_tile_map_config.IfOverlap(camera_rect, trans.position, interval);
	}
}
