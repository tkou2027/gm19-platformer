#pragma once

#include "math/rect.h"
#include "math/vector2.h"
#include "util/list.h"
#include "resource/space_config.h"
#include "resource/physics_layer.h"

namespace foo
{
	class Collider;
	class CollisionTrigger;
	class GameObject;
	class PhysicsActor;
	class PhysicsSystem
	{
	public:
		void Initialize();
		void Finalize();
		void Update();
		void Clear();

		// checks during update
		const List& GetTrails() { return m_trail_list; }
		bool IfOverlap(PhysicsActorLayer layer, Collider* collider) const;
		SpaceId GetSpaceId(Vector2 position);

		// collision detection
		void AddCollisionTrigger(CollisionTrigger* trigger, CollisionTriggerLayer layer);
		bool HandleOverLap(
			const Rect& rect,
			CollisionTestLayer test_layer,
			GameObject* test_obj,
			CollisionTriggerLayer trigger_layer
		);
	private:
		void UpdateData();
		void UpdateMove();
		List m_actor_list;
		List m_solid_list;
		List m_trail_list;
		
		// Collision detection
		List m_collision_triggers[(size_t)CollisionTriggerLayer::MAX];

		// check player overlap
		PhysicsActor* m_layered_actors[(size_t)PhysicsActorLayer::MAX];
	};
}