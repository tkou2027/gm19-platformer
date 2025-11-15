#pragma once
// inspired by the article: Celeste & TowerFall Physics
#include "physics_component.h"
#include "resource/physics_layer.h"

namespace foo
{
	class PhysicsActor : public PhysicsComponent
	{
	public:
		PhysicsActor(Collider* collider, PhysicsActorLayer layer = PhysicsActorLayer::DEFAULT)
			: PhysicsComponent(PhysicsType::ACTOR, collider), m_layer(layer) {}
		void Move(const List& actor_list, const List& solid_list) override;
		void UpdateContacts(const List& actor_list, const List& solid_list);
		bool GetOnGround() const { return m_on_ground; };
		PhysicsActorLayer GetLayer() const { return m_layer; };
	private:
		static constexpr float BIAS{ 0.01f };
		void MoveOneAxis(const List& actor_list, const List& solid_list, int axis, float distance);
		void MoveX(float dx, const List& actor_list, const List& solid_list);
		void MoveY(float dy, const List& actor_list, const List& solid_list);
		bool m_on_ground{ false };
		PhysicsActorLayer m_layer{ PhysicsActorLayer::DEFAULT };
	};
}