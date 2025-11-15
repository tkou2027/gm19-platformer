#include "space_block.h"

#include "component/render_sprite.h"
#include "component/render_tilemap.h"
#include "component/physics_solid.h"
#include "component/physics_trail.h"
#include "physics/tilemap_collider.h"

// TODO
#include "global_context.h"
#include "physics/physics_system.h"
#include "particle_system.h"
#include "util/random.h"
#include "player.h"

namespace foo
{
	SpaceBlock::SpaceBlock(const TileMapConfig& tile_map_config)
		: m_tile_map_config(tile_map_config)
	{
		
		int tile_cnt = m_tile_map_config.rows * m_tile_map_config.cols;
		m_tiles = new Tile[tile_cnt];
		//for (int i = 0; i < tile_cnt; i++)
		//{
		//	m_tiles[i].type = m_tile_map_config.tile_types[i];
		//	m_tiles[i].space_id = space_id;
		//}
		m_tile_map_config.tiles = m_tiles;
		SetRenderComponent(new RenderTileMap(m_tile_map_config));
		SetPhysicsComponent(new PhysicsSolid(new TileMapCollider(m_tile_map_config)));

		m_particle_system = (ParticleSystem*)GetNamedObject(ObjectName::PARTICLE_SYSTEM);
	}

	bool SpaceBlock::IfInsideView(TileMapInterval& interval)
	{
		// TODO: should be safer box
		const Rect& camera_rect = g_global_context.m_render_system->GetCamera().GetBoundingBox();
		Transform2D trans = m_transform.GetTransform2D();
		return m_tile_map_config.IfOverlap(camera_rect, trans.position, interval);
	}

	void SpaceBlock::CheckInsideSpace(const Vector2& pos, int& inside_cnt, int& outside_cnt)
	{
		SpaceId space_id = g_global_context.m_physics_system->GetSpaceId(pos);
		if (space_id == m_tile_map_config.space_id)
		{
			inside_cnt++;
		}
		else
		{
			outside_cnt++;
		}
	}

	void SpaceBlock::GenParticle(const Vector2& tile_pos)
	{
		Vector2 tile_size = m_tile_map_config.tile_size;
		Particle p{};
		p.size = Vector2{ tile_size.x, tile_size.y * 0.4f } *g_random.NextFloatRange(0.1f, 1.0f);
		p.position = tile_pos +
			Vector2{ tile_size.x * g_random.NextFloatRange(-0.4f, 0.4f), tile_size.y * g_random.NextFloatRange(-0.5f, 0.5f) };
		p.velocity = Vector2{ 0.0f, 1.0f } *g_random.NextFloatRange(-0.5f, 0.5f);
		p.life = 10;
		p.space_id = m_tile_map_config.space_id;
		m_particle_system->AddParticle(p);
	}

	void SpaceBlock::Update()
	{
		// m_physics_component->SetActive(false);
		Vector2 position = m_transform.GetTransform2D().position + m_tile_map_config.offset;
		Vector2 tile_size = m_tile_map_config.tile_size;
		TileMapInterval visible_interval{};
		if (!IfInsideView(visible_interval))
		{
			return;
		}
		for (int row = visible_interval.row_start; row < visible_interval.row_end; ++row)
		{
			for (int col = visible_interval.col_start; col < visible_interval.col_end; ++col)
			{
				TileType tile_type = m_tile_map_config.GetTileTypeAt(row, col);
				if (tile_type == TileType::NONE)
				{
					continue;
				}
				// bool inside = true;
				Tile* tile = m_tiles + row * m_tile_map_config.cols + col;
				const Vector2 tile_offset{ col * tile_size.x, row * tile_size.y };
				const Vector2 tile_pos{ position + tile_offset };
				bool inside{ false }, outside{ false };
				int inside_cnt{ 0 }, outside_cnt{ 0 };
				Rect box{ tile_size, tile_pos };
				for (int i = 0; i < 4; i++)
				{
					const Vector2 pos = box.GetCorner(i);
					CheckInsideSpace(pos, inside_cnt, outside_cnt);
					SpaceId space_id = g_global_context.m_physics_system->GetSpaceId(pos);
					inside |= space_id == m_tile_map_config.space_id;
					outside |= space_id != m_tile_map_config.space_id;
				}
				if (inside_cnt > 0 && (outside_cnt > 0 || tile->active))
				{
					ParticleSystem* particle_system = (ParticleSystem*)GetNamedObject(ObjectName::PARTICLE_SYSTEM);

					int num = inside_cnt > 0 && tile->active ? 3 : 1;
					while (num--)
					{
						GenParticle(tile_pos);
					}
				}
				tile->active = inside_cnt <= outside_cnt;

				//BoxCollider box_collider{ tile_size, tile_pos };
				//// box.SetTransform(m_transform.GetTransform2D());
				//// TODO
				//if (g_global_context.m_physics_system->IfOverlap(PhysicsActorLayer::PLAYER, &box_collider))
				//{
				//	GenParticle(tile_pos);
				//	tile->active = false;
				//}

				Rect my_box{ tile_size, tile_pos };
				Player* player = (Player*)GetNamedObject(ObjectName::PLAYER);
				Rect bbox = player->GetBoundingBox();
				//Vector2 size = bbox.GetSize();
				//size.x += 2.0f;
				//bbox.SetSize(size);
				if (bbox.IfOverlap(my_box))
				{
					GenParticle(tile_pos);
					tile->active = false;
					if (m_tile_map_config.space_id == SpaceId::SPACE_A)
					{
						player->stuck = true;
					}
				}
			}
		}
	}
	void SpaceBlock::Finalize()
	{
		// delete[]m_tile_map_config.tiles;
		delete[]m_tiles;
	}
}
