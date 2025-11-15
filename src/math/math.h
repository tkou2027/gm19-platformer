#pragma once

#include <cmath>

class Math {
public:
	static constexpr float EPSILON{ 1e-4f };
	static constexpr float INF{ 1e+30f }; // std::numeric_limits<flaot>::infinity();
	static constexpr float PI{ 3.141592653f };
	static constexpr float HALF_PI{ PI * 0.5f };
	static constexpr float THREE_HALVES_PI{ PI * 1.5f }; // top direction (left handed)
	static constexpr float TWO_PI{ PI * 2.0f };
	static constexpr float DEG_TO_RAD{ PI / 180.0f };

	static float IsZero(float x)
	{
		return x > -EPSILON && x < EPSILON;
	}

	static float Sign(float x)
	{
		return IsZero(x) ? 0.0f : (x > 0.0f ? 1.0f : -1.0f);
	};

	static int Max(int a, int b)
	{
		return a > b ? a : b;
	}

	static int Min(int a, int b)
	{
		return a < b ? a : b;
	}

	static float Max(float a, float b)
	{
		return a > b ? a : b;
	}

	static float Min(float a, float b)
	{
		return a < b ? a : b;
	}

	static float ClampMagnitude(float value, float mag)
	{
		if (value < -mag)
		{
			return -mag;
		}
		else if (value > mag)
		{
			return mag;
		}
		return value;
	}

	static float RoundToStep(float value, float step) {
		return int(value / step) * step;
	}

	static float MoveTowards(float current, float target, float max_delta)
	{
		if (current < target)
		{
			return Min(current + max_delta, target);
		}
		return  Max(current - max_delta, target);
	}

	static float DegToRad(float deg)
	{
		return deg * DEG_TO_RAD;
	}
};
