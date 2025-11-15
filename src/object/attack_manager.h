#pragma once
#include "game_object.h"
#include "util/list.h"
namespace foo
{
	class AttackManager : public GameObject
	{
	public:
		AttackManager();
		void AddPlayerAttack(Attack* attack);
	private:
		static constexpr int MAX_ATTACKS{ 128 };

	};

	class PlayerAttacks
	{
	public:

	private:
		List m_attacks;
	};
}