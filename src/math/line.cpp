#include "line.h"
#include "rect.h"
namespace foo
{
	bool Line::IfOverlap(const Vector2& p) const
	{
		const Vector2 from_center{ p - m_center };
		float dot_normal = Vector2::Dot(from_center, m_normal);
		return dot_normal >= 0.0f;
	}
	bool Line::IfOverlap(const Rect& rect) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (IfOverlap(rect.GetCorner(i)))
			{
				return true;
			}
		}
		return false;
	}
}
