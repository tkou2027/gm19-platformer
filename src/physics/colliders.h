#pragma once
#include "math/transform2d.h"
#include "math/rect.h"
#include "aabb.h"

namespace foo
{
	struct HitRecord
	{
		float t{ Math::INF };
		Vector2 position;

	};

	class Collider
	{
	public:
		enum class ColliderType
		{
			BOX,
			TILE_MAP,
			LIST
		};
		Collider(ColliderType type) : m_type(type) {}
		virtual ~Collider() = default;
		virtual bool Collides(const Collider* collider) const = 0;
		virtual bool Collides(
			const Collider* collider,
			const Vector2& movement,
			HitRecord& hit_record
		) const = 0;
		virtual void SetTransform(const Transform2D& transform) = 0;
		ColliderType GetType() const { return m_type; }
	protected:
		ColliderType m_type;
	};

	class BoxCollider : public Collider
	{
	public:
		BoxCollider(
			const Vector2& size,
			const Vector2& offset = {}
		) : Collider(ColliderType::BOX), m_size(size), m_offset(offset)
		{
			SetTransform({});
		}
		BoxCollider(const Rect& rect)
			: Collider(ColliderType::BOX), m_size(rect.GetSize()), m_offset(rect.GetCenter())
		{
			SetTransform({});
		}
		bool Collides(const Collider* other) const override;
		bool Collides(
			const Collider* other,
			const Vector2& movement,
			HitRecord& hit_record
		) const override;
		void SetTransform(const Transform2D& transform) override;
		const AABB& GetBoundingBox() const { return m_bbox;  };
		const Rect GetRect() const { return Rect{ m_size, m_position + m_offset }; }
	private:
		bool CollidesBox(const BoxCollider* other) const;
		bool CollidesBox(
			const BoxCollider* other,
			const Vector2& movement,
			HitRecord& hit_record
		) const;
		Vector2 m_size;
		Vector2 m_offset;
		// computed bounding box
		AABB m_bbox;
		Vector2 m_position;
	};
}