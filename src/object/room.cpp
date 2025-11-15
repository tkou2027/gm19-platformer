#include "room.h"

#include "tilemap.h"
#include "space_block.h"
#include "rain.h"
#include "fan.h"
#include "enemy.h"
#include "item.h"

namespace foo
{
	Room::Room(const StageConfig& stage_config, int room_id)
		: m_stage_config(stage_config), m_grid_config(stage_config.grids[room_id]),
		m_room_id(room_id), m_scope_id(stage_config.grids[room_id].scope_id)
	{
		m_room_col = m_room_id % StageConfig::GRID_COLS;
		m_room_row = m_room_id / StageConfig::GRID_COLS;
		m_object_state = ObjectState::IDLE;
	}

	void Room::Initialize()
	{
		LoadStageConfig();
		// add objects to list
		AddObject(m_tile_map, this);
		for (int i = 0; i < m_boundaries.GetCount(); i++)
		{
			AddObject((TileMap*)m_boundaries[i], this);
		}
		for (int i = 0; i < m_space_blocks.GetCount(); i++)
		{
			AddObject((SpaceBlock*)m_space_blocks[i], this);
		}
		for (int i = 0; i < m_items.GetCount(); i++)
		{
			AddObject((GameObject*)m_items[i], this);
		}
		SetIdle();
	}

	void Room::Update() {}

	void Room::Finalize()
	{
		// delete[]m_tile_config;
		m_space_blocks.Finalize();
		m_boundaries.Finalize();
		m_items.Finalize();
	}

	void Room::SetActive()
	{
		Transform* child = m_transform.GetChild();
		while (child)
		{
			GameObject* child_obj = child->GetGameObject();
			if (child_obj)
			{
				child_obj->SetActive();
			}
			child = child->GetSibling();
		}
		GameObject::SetActive();
	}

	void Room::SetIdle()
	{
		Transform* child = m_transform.GetChild();
		while (child)
		{
			GameObject* child_obj = child->GetGameObject();
			if (child_obj)
			{
				child_obj->SetIdle();
			}
			child = child->GetSibling();
		}
		// TODO: clear attacks and trails
		GameObject::SetIdle();
	}

	void Room::SetRemoved()
	{
		Transform* child = m_transform.GetChild();
		while (child)
		{
			GameObject* child_obj = child->GetGameObject();
			if (child_obj)
			{
				child_obj->SetRemoved();
			}
			child = child->GetSibling();
		}
		GameObject::SetRemoved();
	}

	Rect Room::GetBoundingBox()
	{
		const Vector2 position{ m_transform.GetTransform2D().position };
		return Rect{ StageConfig::GRID_SIZE, position };
	}

	void Room::LoadStageConfig()
	{
		// transform
		int col = m_room_id % StageConfig::GRID_COLS;
		int row = m_room_id / StageConfig::GRID_COLS;
		const Vector2 position{ col * StageConfig::GRID_SIZE.x, row * StageConfig::GRID_SIZE.y };
		m_transform.SetPosition({ position });
		m_bbox = Rect{ StageConfig::GRID_SIZE, position };

		// load tilemap
		TileMapConfig tile_map_config{};
		FillStageConfigTileMap(m_grid_config, tile_map_config);
		m_tile_map = new TileMap(tile_map_config);
		m_tile_map->SetIdle();

		// neighbor room boundaries
		m_boundaries.Initialize(MAX_BOUNDARY);
		for (int i = 0; i < MAX_BOUNDARY; i++)
		{
			LoadStageConfigBoundary(i);
		}

		// load space blocks
		m_space_blocks.Initialize(MAX_SPACE_BLOCKS);
		LoadStageConfigSpaceBlocks(SpaceId::SPACE_A);
		LoadStageConfigSpaceBlocks(SpaceId::SPACE_B);

		// other items ========
		m_items.Initialize(MAX_ITEMS);
		// load rain
		m_has_rain = m_grid_config.rain_cnt > 0;
		for (int i = 0; i < m_grid_config.rain_cnt; i++)
		{
			const RainConfig& rain_config = m_grid_config.rain_config[i];
			const Vector2 size{
				rain_config.width * StageConfig::TILE_SIZE.x,
				rain_config.height * StageConfig::TILE_SIZE.y
			};
			const Vector2 center{ GetTileOffset(rain_config.index) + (size - StageConfig::TILE_SIZE) * 0.5f };
			Rect rain_box{ size, center };
			Rain* rain = new Rain(m_scope_id, { size, center }, SpaceId::SPACE_A); // TODO: space
			m_items.Add(rain);
		}

		// load fan ====
		for (int i = 0; i < GridConfig::MAX_FAN; i++)
		{
			float fan_index = m_grid_config.fan_point[i];
			if (fan_index < 0)
			{
				break;
			}
			Vector2 offset = GetTileOffset(fan_index, fan_index - floorf(fan_index));
			Fan* fan = new Fan(m_scope_id);
			fan->GetTransform().SetPosition(offset);
			m_items.Add(fan);
			if (m_room_id == 10)
			{
				fan->m_no_focus = true;
			}
		}
		// load enemies ====
		for (int i = 0; i < GridConfig::MAX_ENEMY; i++)
		{
			int enemy_index = m_grid_config.enemy_point[i];
			if (enemy_index < 0)
			{
				break;
			}
			Vector2 offset = GetTileOffset(enemy_index);
			bool key = m_scope_id == 6; // bad
			EnemyShooter* enemy_shooter = new EnemyShooter(SpaceId::SPACE_A, offset, key);
			enemy_shooter->GetTransform().SetPosition(offset);
			m_items.Add(enemy_shooter);
		}

		if (m_grid_config.item != '\0')
		{
			int item_index = m_grid_config.item_index;
			Vector2 offset = GetTileOffset(item_index);
			Item* item = new Item(m_grid_config.item, m_scope_id);
			item->GetTransform().SetPosition(offset);
			m_items.Add(item);
		}

		// load save point
		Vector2 save_offset = GetTileOffset(m_grid_config.save_point);
		save_offset.y += StageConfig::TILE_SIZE.y * 0.5f; // player offset
		m_save_position = save_offset + m_transform.GetTransform2D().position; // world position;
	}

	void Room::FillStageConfigTileMap(const GridConfig& grid_config, TileMapConfig& tile_map_config) const
	{
		tile_map_config.cols = grid_config.TILE_COLS;
		tile_map_config.rows = grid_config.TILE_ROWS;
		tile_map_config.tile_size = StageConfig::TILE_SIZE;
		tile_map_config.SetCenterOffset();
		tile_map_config.space_id = SpaceId::SPACE_DEFAULT;
		tile_map_config.tile_chars = grid_config.tiles;
	}

	void Room::LoadStageConfigBoundary(int offset_index)
	{
		int offset[]{ 0, -1, 0, 1, 0 };
		const int offset_row{ offset[offset_index] };
		const int offset_col{ offset[offset_index + 1] };
		const int next_row{ m_room_row + offset_row };
		const int next_col{ m_room_col + offset_col };
		if (next_col < 0 || next_col >= StageConfig::GRID_COLS
			|| next_row < 0 || next_row >= StageConfig::GRID_ROWS)
		{
			return; // out of bound
		}
		const int next_id{ next_row * StageConfig::GRID_COLS + next_col };
		const GridConfig& grid_config{ m_stage_config.grids[next_id] };

		TileMapConfig tile_map_config{};
		FillStageConfigTileMap(grid_config, tile_map_config);
		TileMap* next_tile_map = new TileMap(tile_map_config, true); // no render
		next_tile_map->SetIdle();
		next_tile_map->GetTransform().SetPosition({
			offset_col * StageConfig::GRID_SIZE.x,
			offset_row * StageConfig::GRID_SIZE.y
			});
		m_boundaries.Add(next_tile_map);
	}

	void Room::LoadStageConfigSpaceBlocks(SpaceId space_id)
	{
		TileMapConfig tile_map_config{};
		tile_map_config.cols = m_grid_config.TILE_COLS;
		tile_map_config.rows = m_grid_config.TILE_ROWS;
		tile_map_config.tile_size = StageConfig::TILE_SIZE;
		tile_map_config.SetCenterOffset();
		tile_map_config.space_id = space_id;
		tile_map_config.tile_chars = m_grid_config.tiles;
		SpaceBlock* space_block = new SpaceBlock(tile_map_config);
		space_block->SetIdle();
		m_space_blocks.Add(space_block);
	}
	Vector2 Room::GetTileOffset(int index, float decimal)
	{
		int col = index % m_grid_config.TILE_COLS;
		int row = index / m_grid_config.TILE_COLS;
		Vector2 save_offset{ (col + 0.5f + decimal) * StageConfig::TILE_SIZE.x, (row + 0.5f) * StageConfig::TILE_SIZE.y };
		Vector2 center_offset = StageConfig::GRID_SIZE * 0.5f;
		return save_offset - center_offset;
	}
}