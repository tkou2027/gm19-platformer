#include "particle_system.h"
#include "component/render_particle.h"

namespace foo
{
	ParticleSystem::ParticleSystem()
	{
		// render
		SetRenderComponent(new RenderParticle(m_particles));
		// set named object
		m_name = ObjectName::PARTICLE_SYSTEM;
		SetNamedObject(m_name, this);
	}

	void ParticleSystem::Update()
	{
		for (int i = 0; i < m_particles.GetCount(); i++)
		{
			if (m_particles[i].life <= 0)
			{
				m_particles.Remove(i);
				i--;
			}
		}
		for (int i = 0; i < m_particles.GetCount(); i++)
		{
			m_particles[i].life--;
			m_particles[i].position = m_particles[i].position + m_particles[i].velocity;
			m_particles[i].size =
			{
				m_particles[i].size.x * m_particles[i].size_velocity.x,
				m_particles[i].size.y * m_particles[i].size_velocity.y
			};
			// m_particles[i].size.x = m_particles[i].size.x * 0.8f;
		}
	}

	void ParticleSystem::AddParticle(const Particle& particle)
	{
		m_particles.Add(particle);
	}
}
