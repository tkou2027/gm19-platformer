#pragma once
#include "game_object.h"
#include "object_pool.h"
#include "util/list.h"
#include "math/circle.h"
#include "physics/collision_trigger.h"
#include "render/sprite_config.h"

namespace foo
{
	class PlayerAttack : public ObjectLightWeight
	{
	public:
		PlayerAttack(const Circle& circle);
		void Update() {
			if (life_countdown <= 0)
			{
				dead = true;
				return;
			}
			--life_countdown;
		}
		bool IfDead() const
		{
			return dead;
		}
		bool HandleOverlap(
			const Rect& rect,
			CollisionTestLayer other_layer,
			GameObject* other_obj = nullptr
		) override;
		void UpdateRenderData() override;
	private:
		// transform
		Transform* m_parent{ nullptr };
		Transform2D m_transform{};
		// render
		const SpriteConfigInput& m_sprite_config_input;
		SpriteState m_sprite_state{};
		// physics
		Circle m_shape{};
		int life_countdown{ 30 };
		bool dead{ false };
	};

	class PlayerAttackManager : public GameObject
	{
	public:
		PlayerAttackManager();
		void Add(PlayerAttack* attack);
		void Update() override;
		void Clear();
		// TODO: ?
		class PlayerAttackCollisionTrigger : public CollisionTrigger
		{
		public:
			PlayerAttackCollisionTrigger(List& attacks) : m_attacks(attacks) {};
			bool HandleOverlap(const Rect& rect, CollisionTestLayer other_layer, GameObject* other_obj = nullptr) override;
		private:
			List& m_attacks;
		};
	private:
		static constexpr int MAX_ATTACKS{ 8 };
		List m_attacks; // list of player attacks
		PlayerAttackCollisionTrigger m_collision_trigger;
	};
}