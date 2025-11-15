#pragma once
#include "physics_component.h"
#include "resource/space_config.h"
namespace foo
{
	class PhysicsTrail : public PhysicsComponent
	{
	public:
		PhysicsTrail(List& trails) : PhysicsComponent(PhysicsType::TRAIL, nullptr), m_trails(trails) { m_active = true;  };
		void Move(const List& actor_list, const List& solid_list) override {};
		SpaceId GetOverlappingSpaceId(const Vector2& pos) const;
	private:
		List& m_trails;
	};
}