#pragma once

namespace foo
{
	enum class PhysicsActorLayer
	{
		DEFAULT,
		PLAYER,
		MAX
	};

	enum class CollisionTriggerLayer
	{
		PLAYER_ATTACK,
		ENEMY_ATTACK,
		MAX
	};

	enum class CollisionTestLayer
	{
		PLAYER,
		ENEMY,
		CUT_TRIGGER
	};
}