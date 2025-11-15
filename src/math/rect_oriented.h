#pragma once
#include "shape.h"

namespace foo
{
	class RectOriented : public Shape
	{
	public:
		RectOriented() = default;
		// ray shape
		RectOriented(const Vector2& origin, const Vector2& target, float thickness);
		RectOriented(const Vector2& origin, const Vector2& target, float length, float thickness);
		bool IfOverlap(const Vector2& p) const override;
		bool IfOverlap(const Rect& rect) const override;
		float GetLength() const { return m_half_size.x * 2.0f; }
		float GetThickness() const { return m_half_size.y * 2.0f; }
		const Vector2& GetHalfSize() const { return m_half_size; }
		const Vector2& GetCenter() const { return m_center; }
		const Vector2& GetAxisX() const { return m_axis_x; }
		void SetThickness(float thickness) { m_half_size.y = thickness * 0.5f; }
	private:
		void InitRay(const Vector2& origin, const Vector2& target, float thickness);
		// position
		Vector2 m_center{};
		// size
		Vector2 m_half_size{};
		// rotation
		Vector2 m_axis_x{};
		Vector2 m_axis_y{};
	};
}