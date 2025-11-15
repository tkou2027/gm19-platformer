#pragma once
#include "util/particle.h"
#include "resource/space_config.h"
#include "component/transform.h"
#include "game_object.h"


namespace foo
{
	class ParticleSystem : public GameObject
	{
	public:
		ParticleSystem();
		void Update() override;
		void AddParticle(const Particle& particle);
		// void AddEmitter();
	private:
		// void UpdateParticle(Particle& particle);
		ParticleList m_particles;
	};
}