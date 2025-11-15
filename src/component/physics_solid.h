#pragma once
// inspired by the article: Celeste & TowerFall Physics
#include "physics_component.h"

namespace foo
{
	class PhysicsSolid : public PhysicsComponent
	{
	public:
		PhysicsSolid(Collider* collider) : PhysicsComponent(PhysicsType::SOLID, collider) {}
		void Move(const List& actor_list, const List& solid_list) override;
	};
}
