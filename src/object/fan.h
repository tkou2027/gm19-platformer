#pragma once

#include "game_object.h"
#include "render/sprite_config.h"
#include "util/tween.h"
#include "util/yoyo.h"

namespace foo
{
	class Player;
	class TrailManager;
	class TrailObjectLine;
	class FanManager;
	class Fan : public GameObject
	{
	public:
		Fan(int scope_id);
		void Initialize() override;
		void Update() override;
		void OnCancelFocus();
		void SetActive() override;
		void SetIdle() override;
		void SetRemoved() override;
		bool m_no_focus{ false };
	private:
		enum class FanState
		{
			IDLE,
			CHARGE,
			BOUNCE
		};
		struct FanConfig
		{
			float lock_distance{ 150.0f };
			int charge_countdown{ 60 };
			int focus_countdown{ 30 };
			int burst_tween_countdown{ 60 };
			int burst_gen_trail_countdown{ 20 };
			float idle_rotation_speed{ 0.002f };
			float bounce_rotation_speed{ 0.002f };
		};
		struct FanStatus
		{
			bool focused{ false };
			int charge{ 0 };
		};

		void UpdateIdle();
		void UpdateCharge();
		void UpdateBounce();
		void Burst();

		void GenTrail();
		void ResetState();
		void ExitBounce();

		bool CheckFocused();

		static constexpr int NUM_BLADE{ 6 };
		static constexpr int NUM_SPRITES{ NUM_BLADE + 2 };

		Vector2 m_blade_size{};
		Transform m_parts_transform[NUM_SPRITES];
		SpriteState m_parts_sprite[NUM_SPRITES];
		// focus animation
		int m_center_sprite_id{ 0 };
		int m_focus_sprite_id{ 0 };
		int m_focus_countdown{ 0 };
		// reference
		FanManager* m_fan_manager{ nullptr };
		TrailManager* m_trail_manager{ nullptr };
		Player* m_player{ nullptr };
		TrailObjectLine* m_trail{ nullptr };

		// id
		SpaceId m_space_id{ SpaceId::SPACE_A };
		int m_scope_id{ -1 };
		// state
		FanState m_state{ FanState::IDLE };
		int m_countdown{ 0 };
		Tween m_tween{};
		Yoyo m_yoyo{ 60 };
		// charge action
		FanConfig m_config{};


		FanStatus m_status{};
		float m_rotation_offset{ 0.0f };
	};
	
	class FanManager : public GameObject
	{
	public:
		FanManager();
		void Update() override;
		void ApplyFocus(Fan* fan);
		void CancelFocus(); // TODO: this is stupid
	private:
		void CheckCancelFocus();
		Fan* m_active_fan{ nullptr };
		// reference
		Player* m_player{ nullptr };
	};
}