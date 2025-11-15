#include "circle.h"
#include "rect.h"

namespace foo
{
	bool Circle::IfOverlap(const Vector2& p) const
	{
		const Vector2 from_center{ p - center };
		float dist = from_center.Length();
		return dist <= radius;
	}
	bool Circle::IfOverlap(const Rect& rect) const
	{
		// TODO: aabb
		const Vector2& rect_center = rect.GetCenter();
		const Vector2& rect_half_size = rect.GetHalfSize();
		// check if point inside rounded rect
		const Vector2 from_center{ rect_center - center };
		const Vector2 from_corner{ abs(from_center.x) - rect_half_size.x, abs(from_center.y) - rect_half_size.y };
		const Vector2 from_corner_positive{ Math::Max(from_corner.x, 0.0f), Math::Max(from_corner.y, 0.0f) };
		// sdf function by Inigo Quilez
		float d = Math::Min(Math::Max(from_corner.x, from_corner.y), 0.0f)
			+ from_corner_positive.Length()
			- radius;
		return d <= 0;
	}
}