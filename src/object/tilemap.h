#pragma once
#include "game_object.h"
#include "resource/tile.h"
#include "resource/tilemap_common.h"
namespace foo
{
	class TileMap : public GameObject
	{
	public:
		TileMap(const TileMapConfig& config, bool no_render = false);
		void Finalize() override;
		const TileMapConfig& GetConfig() const { return m_config; }
	private:
		TileMapConfig m_config;
	};
}