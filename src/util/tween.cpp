// ====================
// Spriteの属性のイーズイン・イーズアウト
// ====================
#include "tween.h"

namespace
{
	float CalcEaseOutBounce(float t)
	{
		constexpr float c4 = (2 * Math::PI) / 3;

		//return t == 0
		//	? 0
		//	: t == 1
		//	? 1
		//	: powf(2, -10 * t) * sinf((t * 10 - 0.75) * c4) + 1;

		// easeOutBack
		constexpr float c1 = 1.70158;
		constexpr float c3 = c1 + 1;

		return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);

		//constexpr float n1{ 7.5625f };
		//constexpr float d1{ 2.75f };

		//if (t < 1.0f / d1) {
		//	return n1 * t * t;
		//}
		//else if (t < 2.0f / d1) {
		//	return n1 * (t -= 1.5 / d1) * t + 0.75f;
		//}
		//else if (t < 2.5f / d1) {
		//	return n1 * (t -= 2.25f / d1) * t + 0.9375f;
		//}
		//else {
		//	return n1 * (t -= 2.625f / d1) * t + 0.984375f;
		//}
	}

	float CalcEaseInOutBack(float t)
	{
		// easeInOutQuad
		return t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;

		//constexpr float c1 = 1.70158;
		//constexpr float c2 = c1 * 1.525;

		//return t < 0.5
		//	? (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
		//	: (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
	}
}

namespace foo
{
	Tween& Tween::Init(GameObject* object, float duration, TWEEN_FUNCTION func, bool loop, bool yoyo)
	{
		m_object = object;
		// duration
		m_duration = duration;
		m_inv_duration = duration != 0 ? 1.0f / duration : 0.0f;
		m_time_count = 0.0f;
		m_time_step = 1.0f; // count by frame
		m_time_direction = 1;
		// function
		m_func = func;
		// loop
		m_loop = loop;
		m_yoyo = yoyo;
		// reset
		m_type = 0;
		m_to_flag = 0;
		m_delta_flag = 0;
		m_delay = 0.0f;
		m_delay_count = 0.0f;
		m_updated = false;
		// use flag
		m_use = true;

		return *this;
	}

	void Tween::Update()
	{
		if (!m_use)
		{
			return;
		}
		// delay
		if (m_delay_count < m_delay)
		{
			m_delay_count += m_time_step;
			return;
		}
		if (!m_updated)
		{
			// first update
			UpdateInit();
			m_updated = true;
		}
		// calculate t
		float t = m_time_count * m_inv_duration;
		if (m_duration == 0)
		{
			t = 1.0f;
		}
		t = m_time_direction < 0 ? 1 - t : t;
		// curve
		//switch (m_func)
		//{
		//case TWEEN_FUNCTION::EASE_IN_QUAD:
		//{
		//	t = t * t;
		//	break;
		//}
		//case TWEEN_FUNCTION::EASE_OUT_QUAD:
		//{
		//	t = 1 - (1 - t) * (1 - t);
		//	break;
		//}
		//case TWEEN_FUNCTION::EASE_OUT_BOUNCE:
		//{
		//	t = CalcEaseOutBounce(t);
		//	break;
		//}
		//}
		t = EaseFunc(m_func, t);
		t = m_time_direction < 0 ? 1 - t : t;

		// tween sprite attributes
		if (m_type & (int)TWEEN_TYPE::POSITION)
		{
			Transform trans = m_object->GetTransform();
			Vector3 position = Vector3::Interpolate(m_transform_start.GetPosition(), m_transform_target.GetPosition(), t);
			trans.SetPosition(position);
			m_object->SetTransform(trans);
		}
		if (m_type & (int)TWEEN_TYPE::SCALE)
		{
			Transform trans = m_object->GetTransform();
			Vector3 scale = Vector3::Interpolate(m_transform_start.GetScale(), m_transform_target.GetScale(), t);
			trans.SetScale(scale);
			m_object->SetTransform(trans);
		}
		if (m_type & (int)TWEEN_TYPE::ROTATION)
		{
			// TODO 2 direction
			Transform trans = m_object->GetTransform();
			float rotation = m_transform_start.GetRotation() * (1 - t) + m_transform_target.GetRotation() * t;
			trans.SetRotation(rotation);
			m_object->SetTransform(trans);
		}

		// update time
		m_time_count += m_time_step * m_time_direction;
		if (m_time_direction < 0 && m_time_count < 0)
		{
			// yoyo
			if (m_loop)
			{
				m_time_count = -m_time_count;
				m_time_direction = 1;
			}
			else
			{
				m_use = false;
			}
		}
		if (m_time_direction > 0 && m_time_count > m_duration)
		{
			if (m_yoyo)
			{
				m_time_count = 2 * m_duration - m_time_count;
				m_time_direction = -1;
			}
			else if (m_loop)
			{
				m_time_count -= m_duration;
			}
			else
			{
				m_use = false;
			}
		}
	}

	void Tween::UpdateInit()
	{
		int mask = 1;
		for (int mask = 1; mask < (int)TWEEN_TYPE::MAX; mask <<= 1)
		{

			bool to_flag = m_to_flag & mask;
			if (!to_flag)
			{
				continue;
			}
			TWEEN_TYPE type = (TWEEN_TYPE)mask;
			bool delta_flag = m_delta_flag & mask;
			const Transform& trans = m_object->GetTransform();
			switch (type)
			{
			case TWEEN_TYPE::POSITION:
			{
				const Vector3& start_position = trans.GetPosition();
				m_transform_start.SetPosition(start_position);
				if (delta_flag)
				{
					m_transform_target.SetPosition(m_transform_target.GetPosition() + start_position);
				}
			}
			case TWEEN_TYPE::SCALE:
			{
				const Vector3& start_scale = trans.GetScale();
				m_transform_start.SetScale(start_scale);
				if (delta_flag)
				{
					const Vector3& target_scale = m_transform_target.GetScale();
					m_transform_target.SetScale({
						target_scale.x * start_scale.x,
						target_scale.y * start_scale.y,
						target_scale.z * start_scale.z
						});
				}
			}
			case TWEEN_TYPE::ROTATION:
			{
				float rotation = trans.GetRotation();
				m_transform_start.SetRotation(rotation);
				if (delta_flag)
				{
					m_transform_target.SetRotation(m_transform_target.GetRotation() + rotation);
				}
			}
			}
		}
	}

	Tween& Tween::SetTweenFromTo(TWEEN_TYPE type, float start, float target)
	{
		// single attribute
		return SetTweenFromTo(type, Vector3{ start }, Vector3{ target });
	}

	Tween& Tween::SetTweenTo(TWEEN_TYPE type, float target_x, float target_y)
	{
		SetTweenFromTo(type, Vector3{}, Vector3{ target_x, target_y });
		m_to_flag |= (int)type;
		return *this;
	}

	Tween& Tween::SetTweenToDelta(TWEEN_TYPE type, float delta_x, float delta_y)
	{
		SetTweenFromTo(type, Vector3{}, Vector3{ delta_x, delta_y });
		m_to_flag |= (int)type;
		m_delta_flag |= (int)type;
		return *this;
	}

	Tween& Tween::SetTweenFromTo(TWEEN_TYPE type, const Vector2& start, const Vector2& target)
	{
		// x and y, z is 0
		SetTweenFromTo(type, Vector3{ start }, Vector3{ target });
		return *this;
	}
	Tween& Tween::SetTweenFromTo(TWEEN_TYPE type, const Vector3& start, const Vector3& target)
	{
		m_type |= (int)type;
		// clear to and delta flag
		m_to_flag &= ~(int)type;
		m_delta_flag &= ~(int)type;
		switch (type)
		{
		case TWEEN_TYPE::POSITION:
		{
			m_transform_start.SetPosition(start);
			m_transform_target.SetPosition(target);
			break;
		}
		case TWEEN_TYPE::POSITION_X:
		case TWEEN_TYPE::POSITION_Y:
		{
			Vector3 position = m_object->GetTransform().GetPosition();
			size_t index = (type == TWEEN_TYPE::POSITION_X) ? 0 : 1;
			position[index] = start.x;
			m_transform_start.SetPosition(position);
			position[index] = target.x;
			m_transform_target.SetPosition(position);
			break;
		}
		case TWEEN_TYPE::SCALE:
		{
			m_transform_start.SetScale(start);
			m_transform_target.SetScale(target);
			break;
		}
		case TWEEN_TYPE::ROTATION:
		{
			// TODOOOO: rotation direction
			m_transform_start.SetRotation(start.x);
			m_transform_target.SetRotation(target.x);
			break;
		}
		}
		return *this;
	}

	Tween& Tween::SetDelay(float delay)
	{
		m_delay = delay;
		m_delay_count = 0.0f;
		m_updated = false;
		return *this;
	}

	Tween& Tween::SetOffset(float offset, int direction)
	{
		m_time_count = offset * m_duration;
		m_time_direction = direction;
		return *this;
	}

	void Tween::SetUsed()
	{
		m_use = true;
	}

	void Tween::SetUnused()
	{
		m_use = false;
	}

	bool Tween::GetUse()
	{
		return m_use;
	}

	float Tween::EaseFunc(TWEEN_FUNCTION function, float t)
	{
		switch (function)
		{
		case TWEEN_FUNCTION::EASE_IN_QUAD:
		{
			return t * t;
		}
		case TWEEN_FUNCTION::EASE_OUT_QUAD:
		{
			return 1 - (1 - t) * (1 - t);
		}
		case TWEEN_FUNCTION::EASE_OUT_BOUNCE:
		{
			return CalcEaseOutBounce(t);
		}
		case TWEEN_FUNCTION::EASE_IN_OUT_BACK:
		{
			return CalcEaseInOutBack(t);
		}
		default:
		{
			return t;
		}
		}
	}

	// TweenPool
	Tween* TweenPool::Create()
	{
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			if (!m_tweens[i].GetUse())
			{
				m_tweens[i].SetUsed();
				return m_tweens + i;
			}
		}
		return nullptr;
	}

	Tween& TweenPool::GetTween(int id)
	{
		if (id >= 0 && id < POOL_SIZE && m_tweens[id].GetUse()) {
			return m_tweens[id];
		}
	}

	void TweenPool::Update()
	{
		for (Tween& tween : m_tweens)
		{
			if (tween.GetUse())
			{
				tween.Update();
			}
		}

	}
}