#pragma once
#include "game_object.h"
#include "math/rect.h"
#include "resource/space_config.h"

namespace foo
{
	class EnemyAttackManager;
	class EnemyAttackRain;
	class ParticleSystem;
	class Rain : public GameObject
	{
	public:
		Rain(int room_id, const Rect& bbox, SpaceId space_id);
		void Update() override;
		void SetActive() override;
		void SetIdle() override;
		void Finalize() override;
	private:
		int GetDensity() const;
		void GenerateParticle(const Rect& bbox, float height_offset = 0);
		int m_room_id{ 0 };
		float m_speed{ 16.0f };
		Rect m_bbox{};
		SpaceId m_space_id{ SpaceId::SPACE_DEFAULT };
		ParticleSystem* m_particle_system{ nullptr };
		EnemyAttackManager* m_attack_manager{ nullptr };
		EnemyAttackRain* m_attack{ nullptr };
	};
}