#include "rain.h"
#include "particle_system.h"
#include "enemy_attack.h"
#include "util/random.h"

namespace foo
{
	Rain::Rain(int room_id, const Rect& bbox, SpaceId space_id)
		: m_room_id(room_id), m_bbox(bbox), m_space_id(space_id)
	{
		m_particle_system = (ParticleSystem*)GetNamedObject(ObjectName::PARTICLE_SYSTEM);
		m_attack_manager = (EnemyAttackManager*)GetNamedObject(ObjectName::ENEMY_ATTACK_MANAGER);
	}

	void Rain::Update()
	{
		Rect world_bbox{ m_bbox.GetSize(), m_bbox.GetCenter() + m_transform.GetTransform2D().position};
		for (int i = 0; i < GetDensity(); i++)
		{
			GenerateParticle(world_bbox);
		}
	}

	void Rain::SetActive()
	{
		// fill rect
		float h = 0.0f;
		float box_h = m_bbox.GetSize().y;
		Rect world_bbox{ m_bbox.GetSize(), m_bbox.GetCenter() + m_transform.GetTransform2D().position };
		while (h < box_h)
		{
			for (int i = 0; i < GetDensity(); i++)
			{
				GenerateParticle(world_bbox, h);
			}
			h += m_speed;
		}

		// attack
		m_attack = new EnemyAttackRain(world_bbox, m_space_id);
		m_attack_manager->Add(m_attack);

		GameObject::SetActive();
	}
	void Rain::SetIdle()
	{
		if (m_attack)
		{
			m_attack->SetDead();
			m_attack = nullptr;
		}
		GameObject::SetIdle();
	}
	void Rain::Finalize()
	{
		if (m_attack)
		{
			m_attack->SetDead();
			m_attack = nullptr;
		}
	}
	int Rain::GetDensity() const
	{
		float density = 4.0f * (m_bbox.GetSize().x / SCREEN_WIDTH);
		int decimal = floorf(density);
		int floating = g_random.NextFloat() < (density - decimal) ? 1 : 0;
		return decimal + floating;
	}
	void Rain::GenerateParticle(const Rect& bbox, float height_offset)
	{
		float t = g_random.NextFloatRange(bbox.GetLeft(), bbox.GetRight());
		t = Math::RoundToStep(t, 12.0f);
		Particle p{};
		p.scope_id = m_room_id;
		p.size = Vector2{ 5.0f , g_random.NextFloatRange(120.0f, 180.0f) };
		p.position = Vector2{ t, bbox.GetTopLeft().y + height_offset - p.size.y };
		p.velocity = { 0.0f, m_speed };
		p.space_id = m_space_id;
		p.is_mask = g_random.NextFloat() < 0.8f;
		if (p.is_mask)
		{
			p.size.x = 3.0f;
		}
		p.life = (bbox.GetSize().y - height_offset) / m_speed;
		m_particle_system->AddParticle(p);
	}
}
