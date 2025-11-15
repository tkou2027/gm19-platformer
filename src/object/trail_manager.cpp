#include "trail_manager.h"
#include "component/render_trail.h"
#include "component/physics_trail.h"
#include "platform/assert.h"
#include "particle_system.h"
namespace foo
{
	// trail manager
	TrailManager::TrailManager()
	{
		m_trails.Initialize(MAX_TRAILS);

		// components
		// graphics
		m_trail_renderer = new RenderTrail(m_trails);
		SetRenderComponent(m_trail_renderer);
		// physics
		m_trail_physics = new PhysicsTrail(m_trails);
		SetPhysicsComponent(m_trail_physics);

		// set named object
		m_name = ObjectName::TRAIL_MANAGER;
		SetNamedObject(m_name, this);

		m_particle_system = (ParticleSystem*)GetNamedObject(ObjectName::PARTICLE_SYSTEM);
	}

	void TrailManager::Finalize()
	{
		m_trails.ClearDelete();
		m_trails.Finalize();
	}

	void TrailManager::Add(TrailObject* trail_object)
	{
		FOO_ASSERT(!!trail_object, "Adding null trail object");
		FOO_ASSERT(!m_trails.Full(), "Trail objects overflow");
		trail_object->SetParticleSystem(m_particle_system);
		m_trails.Add(trail_object);
	}

	void TrailManager::ClearScope(int scope_id)
	{
		for (int i = 0; i < m_trails.GetCount(); i++)
		{
			TrailObject* trail = (TrailObject*)m_trails[i];
			if (trail->GetScopeId() == scope_id || scope_id ==2)
			{
				delete trail;
				m_trails.RemoveOrderedPend(i);
			}
		}
		m_trails.RemoveOrderedApply();
	}

	void TrailManager::Update()
	{
		for (int i = 0; i < m_trails.GetCount(); i++)
		{
			TrailObject* trail = (TrailObject*)m_trails[i];
			trail->Update();
			if (trail->IfDead())
			{
				delete trail;
				m_trails.RemoveOrderedPend(i);
			}
		}
		m_trails.RemoveOrderedApply();
	}
}
