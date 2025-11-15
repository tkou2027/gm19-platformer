#pragma once
#include <cassert>
#include <stddef.h>

class Vector4
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };
	float w{ 0.0f };

	// constructors
	Vector4() = default;
	Vector4(float x_, float y_ = 0.0f, float z_ = 0.0f, float w_ = 0.0f)
		: x(x_), y(y_), z(z_), w(w_) {
	}

	float operator[](size_t i) const
	{
		assert(i < 4);
		return *(&x + i);
	}

	float& operator[](size_t i)
	{
		assert(i < 4);
		return *(&x + i);
	}

	Vector4 operator+(const Vector4& rhs) const
	{
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Vector4 operator-(const Vector4& rhs) const
	{
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	Vector4 operator*(float t) const
	{
		return Vector4{ x * t, y * t, z * t, w * t };
	}
};
