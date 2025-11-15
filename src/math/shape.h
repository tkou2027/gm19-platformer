#pragma once

#include "vector2.h"

namespace foo
{
	class Rect;
	class Shape
	{
	public:
		enum class ShapeType
		{
			// TODO: order
			SHAPE,  // 0.0
			LINE,   // 1.0
			PIE,    // 2.0 
			CIRCLE, // 3.0
			RECT,    // 4.0
			RECT_ORIENTED
		};
		virtual ~Shape() = default;
		virtual bool IfOverlap(const Vector2& p) const = 0;
		virtual bool IfOverlap(const Rect& rect) const = 0;
	};
}