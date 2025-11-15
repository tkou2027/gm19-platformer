#include "physics_actor.h"

#include "math/vector3.h"
#include "global_context.h"
#include "platform/timer.h"
#include "object/game_object.h"

namespace foo
{
	void PhysicsActor::Move(const List& actor_list, const List& solid_list)
	{
		// float delta_time = g_global_context.m_timer->GetFixedDeltaTime();
		float dx = m_velocity.x; // *delta_time;
		float dy = m_velocity.y; // *delta_time;
		MoveOneAxis(actor_list, solid_list, 0, dx);
		MoveOneAxis(actor_list, solid_list, 1, dy);

		//Vector2 movement{dx, dy};
		//float min_t = 1.0f;
		//for (int i = 0; i < solid_list.GetCount(); i++)
		//{
		//	HitRecord hit_rec;
		//	PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
		//	if (m_collider->Collides(solid->GetCollider(), movement, hit_rec))
		//	{
		//		// reset transform
		//		if (hit_rec.t < min_t)
		//		{
		//			min_t = hit_rec.t;
		//		}
		//	}
		//}

		//Transform trans = m_object->GetTransform();
		//movement = movement * min_t;
		//Vector3 pos = trans.GetPosition();
		//pos.x += movement.x * min_t;
		//pos.y += movement.y * min_t;
		//trans.SetPosition(pos);

		//m_object->SetTransform(trans);
		//m_collider->SetTransform(trans.GetTransform2D());
	}

	void PhysicsActor::UpdateContacts(const List& actor_list, const List& solid_list)
	{
		m_on_ground = false;
		for (int i = 0; i < solid_list.GetCount(); i++)
		{
			HitRecord hit_rec;
			PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
			Vector2 movement{ 0.0f, 1.0f };
			if (m_collider->Collides(solid->GetCollider(), movement, hit_rec))
			{
				// TODO: set normal
				m_on_ground = true;
			}
		}
	}

	void PhysicsActor::MoveOneAxis(const List& actor_list, const List& solid_list, int axis, float distance)
	{
		if (axis < 0 || axis >= 2 || Math::IsZero(distance))
		{
			return;
		}
		Vector2 movement{};
		movement[axis] = distance;

		// test hit ====
		float min_t = 1.0f;
		bool hit = false;
		for (int i = 0; i < solid_list.GetCount(); i++)
		{
			HitRecord hit_rec;
			PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
			if (m_collider->Collides(solid->GetCollider(), movement, hit_rec))
			{
				// reset transform
				if (hit_rec.t < min_t)
				{
					hit = true;
					min_t = hit_rec.t;
				}
			}
		}

		// move ====
		Transform trans = m_object->GetTransform();
		Vector3 pos = trans.GetPosition();
		pos[axis] += movement[axis] * min_t;
		if (hit)
		{
			// resolve collision
			pos[axis] += (movement[axis] > 0 ? -BIAS : BIAS);
			// set velocity to zero??
			m_velocity[axis] = 0.0f;
		}
		trans.SetPosition(pos);
		// update transform
		m_object->SetTransform(trans);
		m_collider->SetTransform(trans.GetTransform2D());
	}

	void PhysicsActor::MoveX(float distance, const List& actor_list, const List& solid_list)
	{
		Vector2 movement{ distance, 0 };
		float min_t = 1.0f;
		bool hit = false;
		for (int i = 0; i < solid_list.GetCount(); i++)
		{
			HitRecord hit_rec;
			PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
			if (m_collider->Collides(solid->GetCollider(), movement, hit_rec))
			{
				// reset transform
				if (hit_rec.t < min_t)
				{
					hit = true;
					min_t = hit_rec.t;
				}
			}
		}

		Transform trans = m_object->GetTransform();
		// movement = movement * min_t;
		Vector3 pos = trans.GetPosition();
		pos.x += movement.x * min_t;
		// pos.y += movement.y * min_t;
		if (hit)
		{
			pos.x += (movement.x > 0 ? -0.01 : 0.01);
		}
		if (hit)
		{
			m_velocity.x = 0.0f;
		}
		trans.SetPosition(pos);

		m_object->SetTransform(trans);
		m_collider->SetTransform(trans.GetTransform2D());
		//Transform trans = m_object->GetTransform();
		//Transform2D trans_prev = trans.GetTransform2D();


		//Vector3 pos = trans.GetPosition();
		//pos.x += distance;
		//trans.SetPosition(pos);
		//m_collider->SetTransform(trans.GetTransform2D());
		//for (int i = 0; i < solid_list.GetCount(); i++)
		//{
		//	PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
		//	if (m_collider->Collides(solid->GetCollider()))
		//	{
		//		// reset transform
		//		m_collider->SetTransform(trans_prev);
		//		return;
		//	}
		//}

		//m_object->SetTransform(trans);
	}

	void PhysicsActor::MoveY(float distance, const List& actor_list, const List& solid_list)
	{
		if (Math::IsZero(distance))
		{
			return;
		}
		Vector2 movement{ 0, distance };
		float min_t = 1.0f;
		bool hit = false;
		for (int i = 0; i < solid_list.GetCount(); i++)
		{
			HitRecord hit_rec;
			PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
			if (m_collider->Collides(solid->GetCollider(), movement, hit_rec))
			{
				// reset transform
				if (hit_rec.t < min_t)
				{
					hit = true;
					min_t = hit_rec.t;
				}
			}
		}

		Transform trans = m_object->GetTransform();
		// movement = movement * min_t;
		Vector3 pos = trans.GetPosition();
		// pos.x += movement.x * min_t;
		pos.y += movement.y * min_t;
		if (hit)
		{
			pos.y += (movement.y > 0 ? -0.01 : 0.01);
		}
		if (hit)
		{
			m_velocity.y = 0.0f;
		}
		trans.SetPosition(pos);

		m_object->SetTransform(trans);
		m_collider->SetTransform(trans.GetTransform2D());
		//Transform trans = m_object->GetTransform();
		//Transform2D trans_prev = trans.GetTransform2D();
		//
		//
		//Vector3 pos = trans.GetPosition();
		//pos.y += distance;
		//trans.SetPosition(pos);
		//m_collider->SetTransform(trans.GetTransform2D());
		//for (int i = 0; i < solid_list.GetCount(); i++)
		//{
		//	PhysicsComponent* solid = (PhysicsComponent*)solid_list[i];
		//	if (m_collider->Collides(solid->GetCollider()))
		//	{
		//		// reset transform
		//		m_collider->SetTransform(trans_prev);
		//		return;
		//	}
		//}

		//m_object->SetTransform(trans);
	}
}