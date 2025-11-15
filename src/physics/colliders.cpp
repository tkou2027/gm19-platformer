#include "colliders.h"

namespace foo
{
	bool BoxCollider::Collides(const Collider* other) const
	{
		switch (other->GetType())
		{
		case ColliderType::BOX:
			return CollidesBox((const BoxCollider*)other);
		default:
			return other->Collides(this);
		}
	}
	bool BoxCollider::Collides(const Collider* other, const Vector2& movement, HitRecord& hit_record) const
	{
		switch (other->GetType())
		{
		case ColliderType::BOX:
			return CollidesBox((const BoxCollider*)other, movement, hit_record);
		default:
			// TODO: order
			return other->Collides(this, movement, hit_record);
		}
	}
	void BoxCollider::SetTransform(const Transform2D& transform)
	{
		// TODO: rotation
		float half_width = m_size.x * transform.scale.x * 0.5f;
		float half_height = m_size.y * transform.scale.y * 0.5f;
		m_position = Vector2{ m_offset.x + transform.position.x,  m_offset.y + transform.position.y };
		m_bbox = AABB{
			Interval{m_position.x - half_width, m_position.x + half_width},
			Interval{m_position.y - half_height, m_position.y + half_height}
		};
	}

	bool BoxCollider::CollidesBox(const BoxCollider* other) const
	{
		if (!m_bbox.Overlaps(other->GetBoundingBox()))
		{
			return false;
		}
		// no rotations for now
		return true;
	}

	bool BoxCollider::CollidesBox(const BoxCollider* other, const Vector2& movement, HitRecord& hit_record) const
	{
		AABB other_bbox = other->GetBoundingBox();
		other_bbox.Expand(m_size);

		Ray move_ray{ m_position, movement };
		Interval ray_interval{ 0.0f, 1.0f };
		bool hit = other_bbox.Hit(move_ray, ray_interval);
		if (!hit)
		{
			return false;
		}
		hit_record.t = ray_interval.GetMin();
		hit_record.position = move_ray.At(hit_record.t);
		return true;
	}
}
