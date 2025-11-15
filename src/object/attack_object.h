#pragma once

#include "math/shape.h"
#include "resource/space_config.h"

namespace foo
{
	class Attack
	{
	public:
		Attack() = default;
		virtual void Update() = 0;
		virtual const Shape* GetShapePhysics() = 0;
		virtual bool IfDead() const = 0;
		SpaceId GetSpaceId() const { return m_space_id; }
	protected:
		SpaceId m_space_id{ SpaceId::SPACE_DEFAULT };
	};
}