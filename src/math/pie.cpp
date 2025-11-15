#include "pie.h"
#include "rect.h"

namespace foo
{
	bool Pie::IfOverlap(const Vector2& p) const
	{
		Vector2 from_center{ p - center };
		float dist = from_center.Length();
		if (dist > radius)
		{
			return false;
		}
		if (Math::IsZero(dist))
		{
			// TODO: inner radius
			return true;
		}

		// if with in rotation
		from_center = from_center * (1.0f / dist);
		return IfInRotation(from_center);
	}
	bool Pie::IfOverlap(const Rect& rect) const
	{
		const Vector2& rect_center = rect.GetCenter();
		const Vector2& rect_half_size = rect.GetHalfSize();
		// check if point inside rounded rect
		const Vector2 from_center = rect_center - center;
		const Vector2 from_corner{ abs(from_center.x) - rect_half_size.x, abs(from_center.y) - rect_half_size.y };
		const Vector2 from_corner_positive{ Math::Max(from_corner.x, 0.0f), Math::Max(from_corner.y, 0.0f) };

		float d = Math::Min(Math::Max(from_corner.x, from_corner.y), 0.0f)
			+ from_corner_positive.Length()
			- radius;
		if (d > 0.0f)
		{
			return false;
		}
		// end rounded rect

		// check rotation
		//float dist = from_center.Length();
		//if (Math::IsZero(dist))
		//{
		//	return true;
		//}
		//return IfInRotation(from_center * (1.0f / dist));
		// TODO: this is wrong
		for (int i = 0; i < 4; i++)
		{
			Vector2 from_center = rect.GetCorner(i) - center;
			float dist = from_center.Length();
			if (Math::IsZero(dist))
			{
				return true;
			}
			from_center = from_center * (1.0f / dist);
			if (IfInRotation(from_center))
			{
				return true;
			}
		}
		return false;
	}
	bool Pie::IfInRotation(const Vector2& p_normalized) const
	{
		// rotation check
		// full circle
		if (abs(rotation_delta) >= Math::TWO_PI)
		{
			return true;
		}
		// empty
		float sign = Math::Sign(rotation_delta);
		if (sign == 0.0f)
		{
			return false;
		}

		Vector2 p_start{ cosf(rotation), sinf(rotation) };
		Vector2 p_end{ cosf(rotation + rotation_delta), sinf(rotation + rotation_delta) };

		float sin_start = p_start.x * p_normalized.y - p_start.y * p_normalized.x;
		float sin_end = p_end.x * p_normalized.y - p_end.y * p_normalized.x;
		sin_start *= sign;
		sin_end *= sign;

		if (abs(rotation_delta) >= Math::PI)
		{
			return sin_start >= 0 || sin_end <= 0;
		}

		return sin_start >= 0 && sin_end <= 0;
	}
}
