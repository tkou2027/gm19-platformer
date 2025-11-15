#pragma once

#include "math/trail_shape.h"
#include "space_config.h";

namespace foo
{
	struct Trail
	{
		TrailShape shape{};
		SpaceId space_id{ SpaceId::SPACE_DEFAULT };
	};
}