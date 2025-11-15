#pragma once

#include "math/vector2.h"
#include <cmath>

namespace foo
{
	struct ReflectionConfig
	{
		Vector2 normal{ 0.0f, 1.0f }; // up
		Vector2 center{ 0.5f, 0.5f }; // default center
	};
}