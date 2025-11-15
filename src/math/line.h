#pragma once
#include "shape.h"

namespace foo
{
	class Line : public Shape
	{
	public:
		Line() = default;
		Line(const Vector2& center, const Vector2 normal)
			: m_center(center), m_normal(normal)
		{
			// TODO: normalize
		}
		bool IfOverlap(const Vector2& p) const override;
		bool IfOverlap(const Rect& rect) const override;
	private:
		Vector2 m_center{};
		Vector2 m_normal{ 1.0f, 0.0f }; // right
	};
	
}