#pragma once
#include "math/rect.h"
#include "resource/physics_layer.h"

// stupid collision detection
namespace foo
{
	class CollisionTrigger
	{
	public:
		virtual bool HandleOverlap(const Rect& rect, CollisionTestLayer other, GameObject* other_obj = nullptr) = 0;
	};
}