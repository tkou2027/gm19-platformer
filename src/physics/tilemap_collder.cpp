#include "tilemap_collider.h"
#include "object/tilemap.h"

namespace foo
{
	TileMapCollider::TileMapCollider(TileMapConfig& tile_map_config)
		: Collider{ ColliderType::TILE_MAP }, m_tile_map_config{ tile_map_config } {
	}

	bool TileMapCollider::Collides(const Collider* other) const
	{
		switch (other->GetType())
		{
		case ColliderType::BOX:
			return CollidesBox((const BoxCollider*)other);
		case ColliderType::TILE_MAP:
			return false; // no tilemap-tilemap collision
		default:
			return other->Collides(this);
		}
	}

	bool TileMapCollider::Collides(const Collider* other, const Vector2& movement, HitRecord& hit_record) const
	{
		switch (other->GetType())
		{
		case ColliderType::BOX:
			return CollidesBox((const BoxCollider*)other, movement, hit_record);
		case ColliderType::TILE_MAP:
			return false; // no tilemap-tilemap collision
		default:
			return other->Collides(this);
		}
	}

	void TileMapCollider::SetTransform(const Transform2D& transform)
	{
		m_transform = transform;
	}

	bool TileMapCollider::CollidesBox(const BoxCollider* other) const
	{
		TileMapInterval overlap_interval{};
		Rect other_rect = other->GetRect();
		if (!m_tile_map_config.IfOverlap(other_rect, m_transform.position, overlap_interval))
		{
			return false;
		}

		const Vector2& tile_size = m_tile_map_config.tile_size;
		const Vector2& map_offset = m_tile_map_config.offset;
		BoxCollider tile_collider{ tile_size, m_transform.position };
		Transform2D tile_offset{};
		// Tile* tile = m_tile_map_config.tiles;
		//for (int row = 0; row < m_tile_map_config.rows; ++row)
		//{
		//    for (int col = 0; col < m_tile_map_config.cols; ++col)
		for (int row = overlap_interval.row_start; row < overlap_interval.row_end; ++row)
		{
			for (int col = overlap_interval.col_start; col < overlap_interval.col_end; ++col)
			{
				// if (tile && tile->type != TileType::NONE && tile->active)
				TileType tile_type = m_tile_map_config.GetTileTypeAt(row, col);
				if (tile_type != TileType::NONE)
				{
					tile_offset.position.x = col * tile_size.x + map_offset.x;
					tile_offset.position.y = row * tile_size.y + map_offset.y;
					tile_collider.SetTransform(tile_offset);
					if (tile_collider.Collides(other))
					{
						return true;
					}
				}
				// tile++;
			}
		}
		return false;
	}

	bool TileMapCollider::CollidesBox(const BoxCollider* other, const Vector2& movement, HitRecord& hit_record) const
	{
		float min_t = 2.0f;
		HitRecord tmp;
		const Vector2& tile_size = m_tile_map_config.tile_size;
		const Vector2& map_offset = m_tile_map_config.offset;
		BoxCollider tile_collider{ tile_size, m_transform.position };
		Transform2D tile_offset{};


		// bounding box
		TileMapInterval overlap_interval{};
		// Rect other_rect = other->GetRect();
		AABB other_bbox = other->GetBoundingBox();
		AABB other_bbox_moved = other_bbox + movement;
		AABB other_bbox_join{ other_bbox, other_bbox_moved };
		Rect other_rect = other_bbox_join.GetRect();
		if (!m_tile_map_config.IfOverlap(other_rect, m_transform.position, overlap_interval))
		{
			return false;
		}

		// Tile* tile = m_tile_map_config.tiles;
		//for (int row = 0; row < m_tile_map_config.rows; ++row)
		//{
		//	for (int col = 0; col < m_tile_map_config.cols; ++col)
		//	{
		for (int row = overlap_interval.row_start; row < overlap_interval.row_end; ++row)
		{
			for (int col = overlap_interval.col_start; col < overlap_interval.col_end; ++col)
			{
				// if (tile && tile->type != TileType::NONE && tile->active)
				// TODO
				const Tile* tile = m_tile_map_config.GetTileAt(row, col);
				{
					if (tile && !tile->active)
					{
						continue;
					}
				}
				TileType tile_type = m_tile_map_config.GetTileTypeAt(row, col);
				if (tile_type != TileType::NONE)
				{
					tile_offset.position.x = col * tile_size.x + map_offset.x;
					tile_offset.position.y = row * tile_size.y + map_offset.y;
					tile_collider.SetTransform(tile_offset);
					if (other->Collides((Collider*)&tile_collider, movement, tmp))
					{
						if (min_t > tmp.t)
						{
							min_t = tmp.t;
							hit_record.t = min_t;
							hit_record.position = tmp.position;
						}
						// return true;
					}
				}
				// tile++;
			}
		}
		return min_t <= 1.0f;
	}
}