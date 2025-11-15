#include "tilemap.h"

// components
#include "component/render_tilemap.h";
#include "component/physics_solid.h";
#include "physics/tilemap_collider.h"
namespace foo
{
	TileMap::TileMap(const TileMapConfig& config, bool no_render)
	{
		m_config = config;
		if (!no_render)
		{
			SetRenderComponent(new RenderTileMap(m_config));
		}
		SetPhysicsComponent(new PhysicsSolid(new TileMapCollider(m_config)));
	}

	void TileMap::Finalize()
	{
		// created while loading
		delete[]m_config.tile_types;
	}
}
