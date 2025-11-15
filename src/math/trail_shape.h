#pragma once

#include "shape.h"
#include "circle.h"
#include "pie.h"
#include "rect.h"
#include "line.h"

// since i don't want to manage pointers...
// should be std::variant ?
namespace foo
{
	class TrailShape : public Shape
	{
	public:
		TrailShape(ShapeType type = ShapeType::SHAPE) : m_type{ type } {}

		bool IfOverlap(const Vector2& p) const override
		{
			switch (m_type)
			{
			case ShapeType::CIRCLE:
			{
				return Circle{ center, radius }.IfOverlap(p);
			}
			case ShapeType::PIE:
			{
				return Pie{ center, radius, rotation, rotation_delta }.IfOverlap(p);
			}
			case ShapeType::LINE:
			{
				return Line{ center, normal }.IfOverlap(p);
			}
			default:
			{
				return false;
			}
			}
		}
		bool IfOverlap(const Rect& rect) const override
		{
			switch (m_type)
			{
			case ShapeType::CIRCLE:
			{
				return Circle{ center, radius }.IfOverlap(rect);
			}
			case ShapeType::PIE:
			{
				return Pie{ center, radius, rotation, rotation_delta }.IfOverlap(rect);
			}
			case ShapeType::LINE:
			{
				return Line{ center, normal }.IfOverlap(rect);
			}
			default:
			{
				return false;
			}
			}
		}

		float GetTypeFloat() const { return static_cast<float>(m_type); }

		Vector2 center{};
		Vector2 normal{ 1.0f, 0.0f };
		float radius{ 0.0f };
		float rotation{ 0.0f };
		float rotation_delta{ 0.0f };
	private:
		ShapeType m_type{ ShapeType::SHAPE };
	};
}