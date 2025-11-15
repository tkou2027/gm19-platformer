#pragma once
#include "vector2.h"
#include "shape.h"

namespace foo
{
	class Rect : public Shape
	{
	public:
		Rect(const Vector2& size = { 0, 0 }, const Vector2 center = { 0, 0 }) : m_size(size), m_center(center)
		{
			SetComputed();
		}
		const Vector2& GetSize() const { return m_size; }
		const Vector2& GetHalfSize() const { return m_half_size; }
		const Vector2& GetCenter() const { return m_center; }
		const Vector2& GetTopLeft() const { return m_top_left; }

		void SetSize(const Vector2& size)
		{
			m_size = size;
			SetComputed();
		}
		void SetCenter(const Vector2& center)
		{
			m_center = center;
			SetComputed();
		}
		void SetOffset(const Vector2& top_left)
		{
			m_top_left = top_left;
			m_center = top_left + m_half_size;
		}
		void SetSizeAndCenter(const Vector2& size, const Vector2& center)
		{
			m_size = size;
			m_center = center;
			SetComputed();
		}
		void SetSizeAndTopLeft(const Vector2& size, const Vector2& top_left)
		{
			m_size = size;
			m_half_size = m_size * 0.5f;
			m_top_left = top_left;
			m_center = top_left + m_half_size;
		}
		bool IfOverlap(const Vector2& p) const override
		{
			return (
				p.x >= m_top_left.x && p.x <= m_top_left.x + m_size.x
				&& p.y >= m_top_left.y && p.y <= m_top_left.y + m_size.y
				);
		}
		bool IfOverlap(const const Rect& rect) const override
		{
			const Vector2& other_top_left = rect.GetTopLeft();
			const Vector2& other_size = rect.GetSize();
			return (
				m_top_left.x <= other_top_left.x + other_size.x
				&& m_top_left.x + m_size.x >= other_top_left.x
				&& m_top_left.y <= other_top_left.y + other_size.y
				&& m_top_left.y + m_size.y >= other_top_left.y
				);
		}
		Rect GetCombine(const Rect& other) const
		{
			float left = Math::Min(GetLeft(), other.GetLeft());
			float right = Math::Max(GetRight(), other.GetRight());
			float top = Math::Min(GetTop(), other.GetTop());
			float bottom = Math::Max(GetBottom(), other.GetBottom());
			return Rect{
				Vector2{ right - left, bottom - top },
				Vector2{ right + left, bottom + top } *0.5f,
			};
		}

		// 0 - top left, 1 - top right, 2 - bottom left, 3 - bottom right, 4 - center
		Vector2 GetCorner(int index) const
		{
			if (index >= 4)
			{
				return m_center;
			}
			int x = index % 2;
			int y = index / 2;
			return Vector2{
				m_top_left.x + m_size.x * x,
				m_top_left.y + m_size.y * y
			};
		}
		float GetLeft() const { return m_top_left.x; }
		float GetRight() const { return m_top_left.x + m_size.x; }
		float GetTop() const { return m_top_left.y; }
		float GetBottom() const { return m_top_left.y + m_size.y; }
	private:
		void SetComputed()
		{
			m_half_size = m_size * 0.5f;
			m_top_left = m_center - m_half_size;
		}
		Vector2 m_size;
		// position
		Vector2 m_center;
		// computed
		Vector2 m_top_left;
		Vector2 m_half_size;
	};
}