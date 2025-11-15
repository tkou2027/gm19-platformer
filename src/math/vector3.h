#pragma once
#include <cassert>
#include "vector2.h"

class Vector3
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };

	// constructors
	Vector3() = default;
	Vector3(float x_, float y_ = 0.0f, float z_ = 0.0f) : x(x_), y(y_), z(z_) {}
	Vector3(const Vector2& vec2) : x(vec2.x), y(vec2.y) {}

	float operator[](size_t i) const
	{
		assert(i < 3);
		return *(&x + i);
	}

	float& operator[](size_t i)
	{
		assert(i < 3);
		return *(&x + i);
	}

	Vector3 operator+(const Vector3& rhs) const
	{
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3 operator-(const Vector3& rhs) const
	{
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 operator*(float t) const
	{
		return Vector3{ x * t, y * t, z * t };
	}

	float SquaredLength() const
	{
		return x * x + y * y + z * z;
	}

	static Vector3 Interpolate(const Vector3 from, const Vector3 to, float t)
	{
		return from * (1.0f - t) + to * t;
	}
};
