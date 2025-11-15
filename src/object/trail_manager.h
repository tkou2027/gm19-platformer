#pragma once
#include "game_object.h"
#include "util/list.h"
#include "math/shape.h"
#include "math/trail_shape.h"
#include "resource/space_config.h"
#include "component/physics_trail.h"//TODO
#include "trail_object.h"
namespace foo
{
	class RenderTrail;
	class PhysicsTrail;
	class ParticleSystem;
	class Camera;
	class TrailManager : public GameObject
	{
	public:
		TrailManager();
		void Update() override;
		void Finalize() override;
		void Add(TrailObject* trail_object);
		void ClearScope(int scope_id);
	private:
		static constexpr int MAX_TRAILS{ 128 };
		List m_trails;
		// trail components
		RenderTrail* m_trail_renderer{ nullptr };
		PhysicsTrail* m_trail_physics{ nullptr };
		ParticleSystem* m_particle_system{ nullptr };
		Camera* m_camera{ nullptr };
	};
}
