#include "physics_system.h"

#include "global_context.h"
#include "scene/scene.h"
#include "colliders.h"
#include "collision_trigger.h"
#include "component/physics_component.h"
#include "component/physics_actor.h"
#include "component/physics_trail.h"

namespace foo
{
	void PhysicsSystem::Initialize()
	{
		// TODO: config max bodies
		m_actor_list.Initialize(128);
		m_solid_list.Initialize(128);
		m_trail_list.Initialize(128);
		// colliders
		for (int i = 0; i < (int)CollisionTriggerLayer::MAX; i++)
		{
			m_collision_triggers[i].Initialize(8);
		}
	}

	void PhysicsSystem::Finalize()
	{
		// colliders
		for (int i = 0; i < (int)CollisionTriggerLayer::MAX; i++)
		{
			m_collision_triggers[i].Finalize();
		}
		m_actor_list.Finalize();
		m_solid_list.Finalize();
		m_trail_list.Finalize();
	}

	void PhysicsSystem::Update()
	{
		// collect all bodies in current scene
		UpdateData();
		// update move
		UpdateMove();
	}

	void PhysicsSystem::Clear()
	{
		m_actor_list.Clear();
		m_solid_list.Clear();
		m_trail_list.Clear();
		for (int i = 0; i < (int)CollisionTriggerLayer::MAX; i++)
		{
			m_collision_triggers[i].Clear();
		}
	}

	bool PhysicsSystem::IfOverlap(PhysicsActorLayer layer, Collider* collider) const
	{
		PhysicsActor* actor = m_layered_actors[(size_t)layer];
		if (!actor)
		{
			return false;
		}
		return actor->GetCollider()->Collides(collider);
	}
	SpaceId PhysicsSystem::GetSpaceId(Vector2 position)
	{
		// there should only be one actually...
		if (m_trail_list.Empty())
		{
			return SpaceId::SPACE_DEFAULT;
		}
		const PhysicsTrail* trail_physics = (const PhysicsTrail*)m_trail_list[0];
		return trail_physics->GetOverlappingSpaceId(position);
	}

	// colliders
	void PhysicsSystem::AddCollisionTrigger(CollisionTrigger* trigger, CollisionTriggerLayer layer)
	{
		List& triggers = m_collision_triggers[(size_t)layer];
		FOO_ASSERT(!triggers.Full(), "Collision Layer overflow");
		triggers.Add(trigger);
		// TODO id...
	}

	bool PhysicsSystem::HandleOverLap(
		const Rect& rect, CollisionTestLayer test_layer, GameObject* test_obj, CollisionTriggerLayer trigger_layer)
	{
		List& triggers = m_collision_triggers[(size_t)trigger_layer];
		bool overlap{ false };
		for (int i = 0; i < triggers.GetCount(); i++)
		{
			CollisionTrigger* trigger = (CollisionTrigger*)triggers[i];
			overlap |= trigger->HandleOverlap(rect, test_layer, test_obj);
		}
		return overlap;
	}

	void PhysicsSystem::UpdateData()
	{
		m_actor_list.Clear();
		m_solid_list.Clear();
		m_trail_list.Clear();
		// clear layered objects
		for (PhysicsActor* (&layer) : m_layered_actors)
		{
			layer = nullptr;
		}
		Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
		if (!scene)
		{
			return;
		}
		List& objects = scene->GetGameObjects().GetObjectList();
		for (int i = 0; i < objects.GetCount(); i++)
		{
			GameObject* obj = (GameObject*)objects[i];
			if (!obj->GetActive())
			{
				continue;
			}
			PhysicsComponent* body = obj->GetPhysicsComponent();
			if (!body || !body->GetActive())
			{
				continue;
			}

			// make data list
			switch (body->GetPhysicsType())
			{
			case PhysicsComponent::PhysicsType::ACTOR:
			{
				m_actor_list.Add(body);
				PhysicsActor* actor = (PhysicsActor*)body;
				PhysicsActorLayer actor_layer = actor->GetLayer();
				if (actor_layer != PhysicsActorLayer::DEFAULT)
				{
					m_layered_actors[(size_t)actor_layer] = actor;
				}
				break;
			}
			case PhysicsComponent::PhysicsType::SOLID:
			{
				m_solid_list.Add(body);
				break;
			}
			case PhysicsComponent::PhysicsType::TRAIL:
			{
				m_trail_list.Add(body);
				break;
			}
			}
			// update object transform to collider
			body->UpdateTransform(obj->GetTransform());
		}
	}

	void PhysicsSystem::UpdateMove()
	{
		// update actors
		for (int i = 0; i < m_actor_list.GetCount(); i++)
		{
			PhysicsActor* body = (PhysicsActor*)m_actor_list[i];
			body->Move(m_actor_list, m_solid_list);
			body->UpdateContacts(m_actor_list, m_solid_list);
		}
		// update solids
		for (int i = 0; i < m_solid_list.GetCount(); i++)
		{
			PhysicsComponent* body = (PhysicsComponent*)m_solid_list[i];
			body->Move(m_actor_list, m_solid_list);
		}
	}
}