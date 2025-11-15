#pragma once
#include "math/vector2.h"
#include "math/transform2d.h"
#include "math/rect.h"
#include "util/random.h"

namespace foo
{
	class Camera
	{
	public:
		void Initialize(const Vector2& screen_size)
		{
			m_screen_size = screen_size;
			m_bbox = Rect{ m_screen_size, m_transform.position };

			m_transform_this_frame = {};
			m_transform = {}; // pending camera movement
			m_shake_offset = {};
			m_shake_strength = {};
			m_shake_countdown = 0;
			m_shake_countdown_duration = 0;
			m_in_shake = false;
		}
		void Update()
		{
			m_transform_this_frame = m_transform;
			m_bbox = Rect{ m_screen_size, m_transform.position };
			if (m_in_shake)
			{
				UpdateShake();
			}

		}
		void MoveTo(const Vector2& offset, float speed = 10.0)
		{
			Vector2 movement = offset - m_transform.position;
			float length = movement.Length();
			if (length > speed)
			{
				movement = movement * (speed / length);
			}
			m_transform.position = m_transform.position + movement;
		}
		void SetOffset(const Vector2& offset)
		{
			m_transform.position = offset;
		}
		const Vector2 GetOffset() const
		{
			return m_transform_this_frame.position + m_shake_offset;
		}
		const Vector2& GetOffsetNoShake() const
		{
			return m_transform_this_frame.position;
		}
		const Vector2& GetScreenSize() const
		{
			return m_screen_size;
		}
		const Rect& GetBoundingBox() const
		{
			return m_bbox;
		}
		// screen shake
		void TriggerShake(const Vector2& strength, int duration = 4)
		{
			if (duration <= 0)
			{
				return;
			}
			m_in_shake = true;
			m_shake_countdown_duration = Math::Max(m_shake_countdown_duration, duration);
			m_shake_countdown = m_shake_countdown_duration;
			m_shake_strength = Vector2{
				Math::Max(strength.x, m_shake_strength.x),
				Math::Max(strength.y, m_shake_strength.y)
			};
		}
		void UpdateShake()
		{
			if (m_shake_countdown <= 0)
			{
				m_in_shake = false;
				m_shake_offset = Vector2{ 0.0f, 0.0f };
				m_shake_strength = Vector2{ 0.0f, 0.0f };
				m_shake_countdown_duration = 0.0f;
				return;
			}
			m_shake_countdown--;
			float t = 1.0f - (float)m_shake_countdown / m_shake_countdown_duration;
			float decay = 1.0f - t;
			m_shake_offset.x = sinf(t) * m_shake_strength.x * decay;
			m_shake_offset.y = cosf(t) * m_shake_strength.y * decay;
		}
	private:
		Vector2 m_screen_size;
		Transform2D m_transform_this_frame;
		Transform2D m_transform; // pending camera movement
		Rect m_bbox;
		// screen shake
		Vector2 m_shake_offset{};
		Vector2 m_shake_strength{};
		int m_shake_countdown{ 0 };
		int m_shake_countdown_duration{ 0 };
		bool m_in_shake{ false };
	};
}