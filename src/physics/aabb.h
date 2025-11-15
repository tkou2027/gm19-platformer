#pragma once

#include "ray.h"
#include "interval.h"
#include "math/math.h"
#include "math/vector2.h"
#include "math/rect.h"

namespace foo
{
	class AABB
	{
	public:
		//static const AABB EMPTY, UNIVERSE;
		AABB() {} // The default AABB is empty, since intervals are empty by default.
		AABB(const Interval& x, const Interval& y) : m_x(x), m_y(y)
		{
			PadToMinimums();
		}
		AABB(const AABB& a, const AABB& b)
		{
			m_x = Interval(a.m_x, b.m_x);
			m_y = Interval(a.m_y, b.m_y);
			PadToMinimums();
		}
		AABB(const Vector2& a, const Vector2& b)
		{
			// Treat the two points a and b as extrema for the bounding box, so we don't require a
			// particular minimum/maximum coordinate order.
			m_x = a[0] < b[0] ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
			m_y = a[1] < b[1] ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
			PadToMinimums();
		}
		AABB(const Rect& rect)
		{
			const Vector2& half_size = rect.GetHalfSize();
			const Vector2& center = rect.GetCenter();
			m_x = Interval(center.x - half_size.x, center.x + half_size.x);
			m_y = Interval(center.y - half_size.y, center.y + half_size.y);
			PadToMinimums();
		}
		const Rect GetRect() const
		{
			const Vector2 size{
				m_x.Size(), m_y.Size()
			};
			const Vector2 center{
				(m_x.GetMin() + m_x.GetMax()) * 0.5f,
				(m_y.GetMin() + m_y.GetMax()) * 0.5f,
			};
			return Rect{ size, center };
		}
		const Interval& GetAxisInterval(int axis) const
		{
			return axis == 1 ? m_y : m_x;
		}

		int LongestAxis() const
		{
			float size_x = m_x.Size();
			float size_y = m_y.Size();
			return size_x > size_y ? 0 : 1;
		}

		bool Overlaps(const AABB& other) const
		{
			return m_x.Overlaps(other.m_x) && m_y.Overlaps(other.m_y);
		}

		void Expand(const Vector2& delta)
		{
			m_x = m_x.Expand(delta.x);
			m_y = m_y.Expand(delta.y);
		}

		// hit ray
		bool Hit(const Ray& ray, Interval& ray_interval) const
		{
			const Vector2& origin = ray.GetOrigin();
			const Vector2& dir = ray.GetDirection();
			for (int axis = 0; axis < 2; axis++)
			{
				Interval bounding_interval = GetAxisInterval(axis);

				if (Math::IsZero(dir[axis]))
				{
					// parallel to axis
					if (bounding_interval.Contains(origin[axis]))
					{
						continue;
					}
					return false;
				}

				// get ray-interval intersection
				float axis_dir_inv = 1.0f / dir[axis];
				float t0 = (bounding_interval.GetMin() - origin[axis]) * axis_dir_inv;
				float t1 = (bounding_interval.GetMax() - origin[axis]) * axis_dir_inv;

				// make sure t_min < t_max
				float t_min = Math::Min(t0, t1);
				float t_max = Math::Max(t0, t1);

				// shrink interval min, max
				ray_interval.SetInterval(
					Math::Max(ray_interval.GetMin(), t_min),
					Math::Min(ray_interval.GetMax(), t_max)
				);
				// no overlap when interval gets empty
				if (ray_interval.Empty())
				{
					return false;
				}
			}
			return true;
		}

		AABB operator+(const Vector2& offset) const
		{
			return AABB(m_x + offset.x, m_y + offset.y);
		}

		friend AABB operator+(const Vector2& offset, const AABB& bbox)
		{
			return bbox + offset;
		}


	private:
		void PadToMinimums()
		{
			float delta = Math::EPSILON;
			if (m_x.Size() < delta)
			{
				m_x = m_x.Expand(delta);
			}
			if (m_y.Size() < delta)
			{
				m_y = m_y.Expand(delta);
			}
		}

		Interval m_x, m_y;
	};

	//const AABB AABB::EMPTY = AABB(Interval::empty, Interval::empty);
	//const AABB AABB::UNIVERSE = AABB(Interval::universe, Interval::universe);

}