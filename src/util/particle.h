#pragma once
#include "math/vector2.h"
#include "math/trail_shape.h"
#include "resource/space_config.h"

namespace foo
{
	struct ParticlePreset
	{
		int m_scope_id;
		// shader
		float min_life{ 0.0f };
		float max_life{ 0.0f };
		Vector2 min_size{};
		Vector2 max_size{};
		float min_speed{ 0.0f };
		float max_speed{ 0.0f };
		// emitter
		TrailShape emit_shape{};
		int emit_count{ 0 };
		float rate_per_frame{ 0.0f };
	};

	struct Particle
	{
		int scope_id{ -1 };
		int life{ 0 };
		
		Vector2 size{};
		Vector2 position{};
		Vector2 velocity{};
		Vector2 size_velocity{ 1.0f, 1.0f };
		SpaceId space_id{ SpaceId::SPACE_DEFAULT };
		bool is_mask{ false };
	};

	class ParticleList
	{
	public:
		void Add(const Particle& particle);
		void Remove(int index);
		int GetCount() const;
		Particle& operator[](size_t i)
		{
			return m_data[i];
		}
		const Particle& operator[](size_t i) const
		{
			return m_data[i];
		}
	private:
		static constexpr int MAX_PARTICLES{ 2048 };
		Particle m_data[MAX_PARTICLES];
		int m_count{ 0 };
	};
}