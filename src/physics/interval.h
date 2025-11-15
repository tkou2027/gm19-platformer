#pragma once
// inspred by raytracing the next week
#include "math/math.h"

namespace foo
{

	class Interval
	{
	public:
		//static const float infinity;
		//static const Interval empty, universe;

		Interval() : m_min(Math::INF), m_max(-Math::INF) {};
		Interval(float min, float max) : m_min(min), m_max(max) {};
		// join two intervals
		Interval(const Interval& a, const Interval& b)
		{
			// Create the interval tightly enclosing the two input intervals.
			m_min = a.m_min <= b.m_min ? a.m_min : b.m_min;
			m_max = a.m_max >= b.m_max ? a.m_max : b.m_max;
		}
		float GetMin() const
		{
			return m_min;
		}
		float GetMax() const
		{
			return m_max;
		}
		void SetMin(float min)
		{
			m_min = min;
		}
		void SetMax(float max)
		{
			m_max = max;
		}
		void SetInterval(float min, float max)
		{
			m_min = min;
			m_max = max;
		}
		float Size() const
		{
			return m_max - m_min;
		}
		bool Empty() const
		{
			return m_min > m_max;
		}
		bool Contains(float x) const
		{
			return x >= m_min && x <= m_max;
		}
		bool Surrounds(float x) const
		{
			return x > m_min && x < m_max;
		}
		bool Overlaps(const Interval& other) const
		{
			return m_min <= other.m_max && m_max >= other.m_min;
		}
		float Clamp(float x) const
		{
			return x < m_min ? m_min : (x > m_max ? m_max : x);
		}
		Interval Expand(float delta) const
		{
			auto padding = delta / 2;
			return Interval(m_min - padding, m_max + padding);
		}
		Interval operator+(float displacement) const
		{
			return Interval(m_min + displacement, m_max + displacement);
		}

		friend Interval operator+(float displacement, const Interval& ival)
		{
			return ival + displacement;
		}
	private:
		float m_min{ Math::INF };
		float m_max{ -Math::INF };
	};
	//const float Interval::infinity = 100000.0f; // TODO
	//const Interval Interval::empty = Interval(+Interval::infinity, -Interval::infinity);
	//const Interval Interval::universe = Interval(-Interval::infinity, Interval::infinity);
	//Interval operator+(const Interval& ival, float displacement)
	//{
	//	return Interval(ival.m_min + displacement, ival.m_max + displacement);
	//}

	//Interval operator+(float displacement, const Interval& ival)
	//{
	//	return ival + displacement;
	//}
}