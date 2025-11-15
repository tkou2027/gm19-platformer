#pragma once

#include "component.h"
#include "transform.h"
#include "math/vector2.h"
#include "util/list.h"
#include "physics/colliders.h"

namespace foo
{
	class PhysicsComponent : public Component
	{
	public:
		enum class PhysicsType
		{
			ACTOR,
			SOLID,
			TRAIL
		};
		PhysicsComponent(PhysicsType physics_type, Collider* collider) :
			m_physics_type{ physics_type }, m_collider{ collider }
		{
			m_type = ComponentType::PHYSICS;
		}
		virtual ~PhysicsComponent()
		{
			delete m_collider;
		};
		virtual void Move(const List& actor_list, const List& solid_list) = 0;

		PhysicsType GetPhysicsType() const { return m_physics_type; }
		const Vector2& GetVelocity() const { return m_velocity; }
		const Collider* GetCollider() const { return m_collider; }
		Collider* GetCollider() { return m_collider; }
		void SetVelocity(const Vector2& velocity) { m_velocity = velocity; }
		void SetVelocityX(float velocity_x) { m_velocity.x = velocity_x; }
		void SetVelocityY(float velocity_y) { m_velocity.y = velocity_y; }
		void SetCollider(Collider* collider, bool delete_prev = true)
		{
			if (delete_prev)
			{
				delete m_collider;
			}
			m_collider = collider;
		}
		void UpdateTransform(Transform& transform)
		{
			if (m_collider)
			{
				m_collider->SetTransform(transform.GetTransform2D());
			}
		}
	protected:
		PhysicsType m_physics_type;
		Vector2 m_velocity;
		Collider* m_collider{ nullptr };
	};
}