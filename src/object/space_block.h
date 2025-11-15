#pragma once

#include "game_object.h"
#include "resource/space_config.h"
#include "resource/tilemap_common.h"

namespace foo
{
	class ParticleSystem;
	class SpaceBlock : public GameObject
	{
	public:
		SpaceBlock(const TileMapConfig& tile_map_config);
		void Update() override;
		void Finalize() override;
	private:
		// bool IfInsideView(int& col_start, int& col_end, int& row_start, int& row_end);
		bool IfInsideView(TileMapInterval& interval);
		void CheckInsideSpace(const Vector2& pos, int& inside_cnt, int& outside_cnt);
		void GenParticle(const Vector2& tile_pos);

		TileMapConfig m_tile_map_config;
		Tile* m_tiles{ nullptr };//[1024];
		ParticleSystem* m_particle_system{ nullptr };
	};
}