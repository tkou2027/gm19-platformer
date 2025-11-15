#pragma once

#include "vector2.h"

namespace foo
{
	struct Transform2D
	{
		Vector2 position{ 0.0f, 0.0f };
		Vector2 scale{ 1.0f, 1.0f };
		float rotation{ 0.0f };
	};
}