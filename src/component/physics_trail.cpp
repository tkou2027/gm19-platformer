#include "physics_trail.h"
//// #include "resource/trail.h"
#include "object/trail_manager.h"
namespace foo
{
	// PhysicsTrail(List& trails) : PhysicsComponent(PhysicsType::TRAIL, nullptr), m_trails(trails) { m_active = true; };

	SpaceId PhysicsTrail::GetOverlappingSpaceId(const Vector2& pos) const
	{
		for (int i = m_trails.GetCount() - 1; i >= 0; i--)
		{
			TrailObject* trail = (TrailObject*)m_trails[i];
			TrailShape trail_shape = trail->GetShapePhysics();
			if (trail_shape.IfOverlap(pos))
			{
				return trail->GetSpaceId();
			}
		}
		return SpaceId::SPACE_B; // SpaceId::SPACE_DEFAULT;
	}
}