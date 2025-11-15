#pragma once
#include "shape.h"

namespace foo
{
	class Pie : public Shape
	{
	public:
		Pie() = default;
		Pie(const Vector2& center, float radius,
			float rotation, float rotation_delta
		) : center(center), radius(radius), rotation(rotation), rotation_delta(rotation_delta) {
		}
		bool IfOverlap(const Vector2& p) const override;
		bool IfOverlap(const Rect& rect) const override;
		Vector2 center{};
		float radius{ 0.0f };
		float rotation{ 0.0f };
		float rotation_delta{ 0.0f };
	private:
		bool IfInRotation(const Vector2& p) const;
	};
}