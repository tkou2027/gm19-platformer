#pragma once
#include "render_component.h"
#include "util/list.h"
#include "util/particle.h"

namespace foo
{
	class RenderParticle : public RenderComponent
	{
	public:
		RenderParticle(ParticleList& particle_list);
		void UpdateRenderData() override;
	private:
		ParticleList& m_particle_list;
	};
}