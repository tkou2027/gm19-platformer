#include "render_particle.h"
#include "global_context.h"
#include "render/render_system.h"

namespace foo
{
	RenderParticle::RenderParticle(ParticleList& particle_list)
		: m_particle_list(particle_list) {}

	void RenderParticle::UpdateRenderData()
	{
		for (int i = 0; i < m_particle_list.GetCount(); i++)
		{
			RenderSystem* render_system = g_global_context.m_render_system;
			render_system->GetParticleData().Add(m_particle_list[i]);
		}
	}
}

