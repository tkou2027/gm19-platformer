#pragma once
#include "game_object.h"
#include "util/tween.h"
#include "resource/space_config.h"
#include "math/trail_shape.h"
#include "render/sprite_config.h"

namespace foo
{
	class Player;
	class Blade;
	class Sword : public GameObject
	{
	public:
		struct SwordConfig
		{
			int pre_charge_countdown{ 16 };
			int slash_countdown{ 12 };
			int charge_countdown{ 24 };
			int charge_slash_countdown{ 20 };
		};

		enum class SwordState
		{
			IDLE,
			PRE_CHARGE,
			CHARGE,
			SLASH,
			CHARGE_SLASH
		};

		Sword();
		void Update() override;

		void SetPlayer(Player* players);
		// animations
		void StartPreCharge();
		void StartCharge();
		void StartSlash();
		void StartChargeSlash();
		void StartIdle();

		void SetDirection(float direction);
		float GetDirection() const { return m_direction; }

		bool IfLockMovement() const;
		bool IfLockRotation() const;

		const SwordConfig* GetSwordConfig() const { return &m_config; }
	private:
		static constexpr int NUM_SPRITES{ 1 };
		void DoSlash();
		void DoChargeSlash();

		SpriteState m_sprites[NUM_SPRITES];

		SwordConfig m_config{};
		SwordState m_state{ SwordState::IDLE };
		int m_state_countdown{ 0 };

		Vector2 m_blade_size{ 120.0f, 20.0f };
		Player* m_player{ nullptr };
		Blade* m_blade{ nullptr };
		
		SpaceId m_space_id{ SpaceId::SPACE_A };
		float m_direction{ 1.0f };
	};
}