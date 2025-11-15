#pragma once
#include "colliders.h"
#include "math/vector2.h"
#include "resource/tilemap_common.h"
namespace foo
{
	class TileMapCollider : public Collider
	{
	public:
		TileMapCollider(TileMapConfig& tile_map_config);
		bool Collides(const Collider* other) const override;
		bool Collides(
			const Collider* other,
			const Vector2& movement,
			HitRecord& hit_record
		) const override;
		void SetTransform(const Transform2D& transform) override;
	private:
		bool CollidesBox(const BoxCollider* other) const;
		bool CollidesBox(
			const BoxCollider* other,
			const Vector2& movement,
			HitRecord& hit_record
		) const;
		Transform2D m_transform{};
		TileMapConfig& m_tile_map_config;
	};
}