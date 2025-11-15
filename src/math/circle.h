#pragma once
#include "shape.h"

namespace foo
{
	class Circle : public Shape
	{
	public:
		Circle() = default;
		Circle(const Vector2& center, float radius) : center(center), radius(radius) {}
		bool IfOverlap(const Vector2& p) const override;
		bool IfOverlap(const Rect& rect) const override;
		Vector2 center{};
		float radius{ 0.0f };
	};
}