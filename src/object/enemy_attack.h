#pragma once
#include "game_object.h"
#include "object_pool.h"
#include "render/sprite_config.h"
#include "math/rect_oriented.h"
#include "resource/space_config.h"

namespace foo
{
	class Player;
	class EnemyAttackRay : public ObjectLightWeight
	{
	public:
		EnemyAttackRay(const Vector2& target, const Vector2& origin, SpaceId space_id);
		void UpdateRenderData() override;
		void SetShape(const Vector2& target, const Vector2& origin);
		bool HandleOverlap(const Rect& other_shape, CollisionTestLayer other_layer, GameObject* other_obj) override;
		void Update() override;
		bool IfDead() const override;
		void SetActive();
		void SetDead() { m_state = EnemyAttackRayState::DEAD; }
	private:
		enum class EnemyAttackRayState
		{
			IDLE,
			ACTIVE,
			DEAD
		};
		SpaceId m_space_id;
		Transform m_transform;
		SpriteState m_sprite;
		RectOriented m_physics_shape;
		EnemyAttackRayState m_state{ EnemyAttackRayState::IDLE };
		int attack_countdown{ 60 };
	};

	class EnemyAttackRain : public ObjectLightWeight
	{
	public:
		EnemyAttackRain(const Rect& bbox, SpaceId space_id) : m_bbox(bbox), m_space_id(space_id) {};
		bool HandleOverlap(const Rect& other_shape, CollisionTestLayer other_layer, GameObject* other_obj) override;
		void SetDead() { m_dead = true; }
		bool IfDead() const override { return m_dead; }
	private:
		bool m_dead{ false };
		Rect m_bbox;
		SpaceId m_space_id;
	};

	class EnemyAttackManager : public GameObject
	{
	public:
		EnemyAttackManager();
		void Add(ObjectLightWeight* attack);
		void Update() override;
		// TODO: ?
		class EnemyAttackCollisionTrigger : public CollisionTrigger
		{
		public:
			EnemyAttackCollisionTrigger(List& attacks) : m_attacks(attacks) {};
			bool HandleOverlap(const Rect& rect, CollisionTestLayer other, GameObject* other_obj = nullptr) override;
		private:
			List& m_attacks;
		};
	private:
		static constexpr int MAX_ATTACKS{ 32 };
		List m_attacks; // list of enemy attacks
		EnemyAttackCollisionTrigger m_collision_trigger;
	};
}