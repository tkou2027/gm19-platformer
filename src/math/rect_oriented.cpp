#include "rect_oriented.h"
#include "rect.h"

namespace foo
{
	RectOriented::RectOriented(const Vector2& origin, const Vector2& target, float thickness)
	{
		InitRay(origin, target, thickness);
	}
	RectOriented::RectOriented(const Vector2& origin, const Vector2& target, float length, float thickness)
	{
		const Vector2& from_origin{ target - origin };
		float semi_length = from_origin.Length();
		if (Math::IsZero(semi_length))
		{
			InitRay(origin, target, thickness);
			return;
		}
		const Vector2 real_target = origin + from_origin * (length / semi_length);
		InitRay(origin, real_target, thickness);
	}
	bool RectOriented::IfOverlap(const Vector2& p) const
	{
		const Vector2 from_center{ p - m_center };
		float proj_x = Vector2::Dot(from_center, m_axis_x);
		float proj_y = Vector2::Dot(from_center, m_axis_y);
		return abs(proj_x) <= m_half_size.x && abs(proj_y) <= m_half_size.y;
	}

	bool RectOriented::IfOverlap(const Rect& rect) const
	{
		// TODO: this is wrong
		const Vector2& center{ rect.GetCenter() };
		return IfOverlap(center);
	}
	void RectOriented::InitRay(const Vector2& origin, const Vector2& target, float thickness)
	{
		m_center = (target + origin) * 0.5f;

		const Vector2& from_origin{ target - origin };
		// size
		float length = from_origin.Length();
		m_half_size.x = length * 0.5f;
		m_half_size.y = thickness * 0.5f;

		// rotation
		m_axis_x = from_origin.GetNormalized(length);
		m_axis_y = Vector2{ -m_axis_x.y, m_axis_x.x };
	}
}
