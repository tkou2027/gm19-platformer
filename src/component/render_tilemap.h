#pragma once
#include <string>
#include "render_component.h"
#include "render/render_system.h"
#include "resource/tilemap_common.h"
namespace foo
{
	class TileMap;
	class RenderTileMap : public RenderComponent
	{
	public:
		RenderTileMap(TileMapConfig& tile_map_config);
		void UpdateRenderData() override;
	private:
		bool IfInsideView(TileMapInterval& interval) const;
		TileMapConfig& m_tile_map_config;
	};
}