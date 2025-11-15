#pragma once

#include "colliders.h"
#include "util/list.h"

namespace foo
{
	class ListCollider : public Collider
	{
	public:
		// this should be a template but since we havene't learn that...
		enum class ListType
		{
			RECT,
			COLLIDER
		};
		ListCollider(ListType list_type, List* list = nullptr)
			: Collider(ColliderType::LIST), m_list_type(list_type), m_list(list) {};
		// collider common
		bool Collides(const Collider* other) const override;
		bool Collides(
			const Collider* other,
			const Vector2& movement,
			HitRecord& hit_record
		) const override;
		void SetTransform(const Transform2D& transform) override;
		// list operations
		void SetList(List* list);
	private:
		ListType m_list_type{ ListType::COLLIDER };
		List* m_list{ nullptr };
		List m_colliders;
	};
}