#include "trail_object.h"

#include "util/tween.h"
#include "util/random.h"
#include "global_context.h"
#include "render/render_system.h"
#include "particle_system.h"

namespace foo
{
	// circle ---
	TrailObjectCircle::TrailObjectCircle(SpaceId space_id, int scope_id, const TrailObjectCircleConfig& config)
		: TrailObject(space_id, scope_id), m_config(config)
	{
		m_shape.center = m_config.center;
		m_shape.radius = m_config.radius;
		m_state = CircleTrailState::EXPAND;
		m_countdown = m_config.expand_frames;
	}

	void TrailObjectCircle::Update()
	{
		switch (m_state)
		{
		case CircleTrailState::EXPAND:
		{
			if (m_countdown == 0)
			{
				m_state = CircleTrailState::YOYO;
				m_countdown = m_config.yoyo_frames;
				break;
			}
			float t = (m_config.expand_frames - m_countdown) / (float)m_config.expand_frames;
			m_shape.radius = m_config.radius
				+ m_config.expand_radius * Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_OUT_QUAD, t);
			m_countdown--;
			break;
		}
		case CircleTrailState::YOYO:
		{
			if (m_countdown == 0)
			{
				m_state = CircleTrailState::SHRINK;
				m_countdown = m_config.shrink_frames;
				break;
			}
			float t = (m_config.yoyo_frames - m_countdown) / (float)m_config.yoyo_frames;
			t = (t <= 0.5f ? (0.5f - t) : (t - 0.5f)) * 2.0f;
			m_shape.radius = m_config.radius
				+ m_config.expand_radius * Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_IN_OUT_BACK, t);
			m_countdown--;
			break;
		}
		case CircleTrailState::SHRINK:
		{
			m_shape.radius *= 0.8f;
			if (m_countdown == 0 || m_shape.radius < m_config.min_radius)
			{
				m_shape.radius = 0.0f;
				m_state = CircleTrailState::DEAD;
				break;
			}
			m_countdown--;
			break;
		}
		}
	}

	void TrailObjectCircle::Destroy()
	{
		m_shape.radius = 0.0f;
		m_state = CircleTrailState::DEAD;
	}

	const TrailShape& TrailObjectCircle::GetShapeRender()
	{
		return m_shape; // TODO
	}

	const TrailShape& TrailObjectCircle::GetShapePhysics()
	{
		return m_shape;
	}


	// Line ----
	TrailObjectLine::TrailObjectLine(SpaceId space_id, int scope_id,
		const TrailObjectLineConfig& config, Transform* transform_parent)
		: TrailObject(space_id, scope_id), m_config(config), m_transform_parent(transform_parent)
	{
		m_shape.normal = m_config.normal;
		if (transform_parent)
		{
			m_shape.center = transform_parent->GetTransform2D().position;
		}
		else
		{
			m_shape.center = m_config.center;
		}
	}
	void TrailObjectLine::Update()
	{
		//Controller* controller = g_global_context.m_controller;
		//float rotation_z = controller->GetControllerLeftAngle().z * Math::TWO_PI * 2.0f;
		//m_shape.normal = { cos(rotation_z), sin(rotation_z) };
		// m_shape.normal = { 1.0f, 1.0f };
		// TODO
		//if (m_life_countdown <= 0)
		//{
		//	Destroy();
		//	// m_dead = true;
		//}
		//--m_life_countdown;
	}
	const TrailShape& TrailObjectLine::GetShapeRender()
	{
		return m_shape;
	}
	const TrailShape& TrailObjectLine::GetShapePhysics()
	{
		return m_shape;
	}
	void TrailObjectLine::Destroy()
	{
		m_dead = true;
		GenDestroyParticle();
	}
	void TrailObjectLine::GenDestroyParticle()
	{
		Camera& camera = g_global_context.m_render_system->GetCamera();
		Rect camera_bbox = camera.GetBoundingBox(); // TODO move
		Vector2 view_size = camera_bbox.GetSize();
		float grid_step = 128.0f;
		int cols = view_size.x / grid_step;
		int rows = view_size.y / grid_step;
		for (int col = 0; col < cols; col++)
		{
			for (int row = 0; row < rows; row++)
			{
				
				const Vector2 position{
					camera_bbox.GetTopLeft()
						+ Vector2{ (col + 0.5f) * grid_step, (row + 0.5f) * grid_step }
				};
				if (!m_shape.IfOverlap(position))
				{
					continue;
				}
				int num = g_random.NextInt(0, 4);
				while (--num > 0) // must > 0 oherwist 0 never
				{
					const Vector2 offset = Vector2{ g_random.NextFloatRange(-0.5f, 0.5f), g_random.NextFloatRange(-0.5f, 0.5f) } * grid_step;
					Particle p{};
					p.size = Vector2{ g_random.NextFloatRange(0.2, 0.3f), g_random.NextFloatRange(0.2, 0.3f) } * grid_step;
					p.position = position + offset;
					p.velocity = { 0.0f, 20.0f * g_random.NextFloat() };
					p.size_velocity = { 0.5f, 1.4f };
					p.life = 4;
					p.space_id = m_space_id;
					m_particle_system->AddParticle(p);
				}
			}
		}
	}
}
