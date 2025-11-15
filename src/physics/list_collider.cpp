#include "list_collider.h"

namespace foo
{
	bool ListCollider::Collides(const Collider* other) const
	{
		if (!m_list)
		{
			return false;
		}
		for (int i = 0; i < m_list->GetCount(); i++)
		{
			Collider* collider = (Collider*)((*m_list)[i]);
			if (collider->Collides(other))
			{
				return true;
			};
		}
	}

	bool ListCollider::Collides(const Collider* other, const Vector2& movement, HitRecord& hit_record) const
	{
		if (!m_list)
		{
			return false;
		}
		bool hit{ false };
		HitRecord tmp_record{};
		for (int i = 0; i < m_list->GetCount(); i++)
		{
			Collider* collider = (Collider*)((*m_list)[i]);
			if (other->Collides(collider, movement, tmp_record))
			{
				hit = true;
				if (tmp_record.t < hit_record.t)
				{
					// TODO: better copy?
					hit_record = tmp_record;
				}
			};
		}
		return hit;
	}

	void ListCollider::SetTransform(const Transform2D& transform)
	{
		if (!m_list)
		{
			return;
		}
		for (int i = 0; i < m_list->GetCount(); i++)
		{
			Collider* collider = (Collider*)((*m_list)[i]);
			collider->SetTransform(transform);
		}
	}

	void ListCollider::SetList(List* list)
	{
		if (m_list_type == ListType::COLLIDER)
		{
			// just a pointer, not updating m_colliders
			m_list = list;
			return;
		}
		// create colliders
		if (m_list_type == ListType::RECT)
		{
			// delete previous colliders
			m_colliders.ClearDelete();
			if (!list)
			{
				// empty
				m_list = nullptr;
				return;
			}
			// create colliders
			m_colliders.Initialize(list->GetCount());
			for (int i = 0; i < list->GetCount(); i++)
			{
				Rect* rec = (Rect*)((*list)[i]);
				BoxCollider* box = new BoxCollider(*rec);
				m_colliders.Add((void*)box);
			}
			m_list = &m_colliders;
		}
	}
}