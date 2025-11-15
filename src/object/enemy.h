#pragma once

#include "game_object.h"
#include "render/sprite_config.h"
#include "resource/space_config.h"
#include "util/yoyo.h"
#include "util/tween.h"

namespace foo
{
	class Player;
	class EnemyAttackManager;
	class EnemyAttackRay;
	class EnemyShooter : public GameObject
	{
	public:
		struct EnemyShooterConfig
		{
			float search_radius{ 400.0f };
			int aim_countdown{ 30 };
			int delay_countdown{ 20 };
			int attack_countdown{ 60 };
			int cooldown_countdown{ 60 };
			int hurt_countdown{ 16 };

			float default_size{ 80.0f };
			float default_size_eye{ 64.0f };
			float default_size_body{ 84.0f };
			float float_size_body{ 10.0f };
			float floating_range{ 25.0f };
		};
		EnemyShooter(SpaceId space_id, const Vector2& initial_position, bool key = false);
		void Update() override;
		void SetActive() override;
		void SetIdle() override;
		void SetRemoved() override;
		void SetKey() { m_key = true; }
	private:
		static constexpr int NUM_SPRITES{ 3 };
		enum class EnemyShooterState
		{
			IDLE,
			AIM,
			ATTACK_DELAY,
			ATTACK,
			COOLDOWN,
			HURT,
			DEAD
		};
		// state ===
		void EnterState(EnemyShooterState state);
		void UpdateIdle();
		void UpdateAim();
		void UpdateCoolDown();
		void UpdateAimAnimation();
		void UpdateBodyAnimation();
		//void UpdateAttackDelay();
		//void UpdateAttack();	
		void UpdateCountdown(EnemyShooterState next_state);
		void CheckHurt();
		EnemyShooterState m_state{ EnemyShooterState::IDLE };
		int m_state_countdown{ 0 };
		Yoyo m_yoyo{ 60 };

		// helpers ===
		bool GetTarget();
		void InitializeAttack();
		void UpdateAttackAim();
		void DoHurtEffect();

		EnemyShooterConfig m_config{};
		SpaceId m_space_id{ SpaceId::SPACE_A };
		// render
		SpriteState m_sprites[NUM_SPRITES]{};
		// Transform m_parts_transform[NUM_SPRITES]{};
		int m_eye_sprite_index{ 0 };
		int m_body_sprite_index{ 0 };
		Vector2 m_initial_position{};
		// reference
		Player* m_player{ nullptr };
		EnemyAttackManager* m_attack_manager{ nullptr };
		EnemyAttackRay* m_attack{ nullptr };
		bool m_key{ false };
	};
}