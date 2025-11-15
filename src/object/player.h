#pragma once
#include "game_object.h"
#include "component/render_sprite.h"
#include "resource/tile.h"
#include "resource/stage_config.h"
#include "sword.h" // sword config
namespace foo
{
	// class Sword;
	class TrailManager;
	class PlayerAttackManager;
	class Stage;
	class Player : public GameObject
	{
	public:
		Player();
		void Update() override;
		enum class PlayerState
		{
			TRANSFER,
			ACTIVE,
		};
		struct PlayerCollisionInfo
		{
			bool hurt{ false };
			bool in_rain{ false };
		};
		void SetSword(Sword* sword) {
			m_sword = sword; // ??
			m_sword_config = sword->GetSwordConfig();
		}
		void SetStage(Stage* stage)
		{
			m_stage = stage;
		}
		bool GetPlayerActive() const { return m_state == PlayerState::ACTIVE;  }
		bool IfPlayerInDistance(const Vector2& from, float distance);
		Vector2 GetPlayerPositionCenter();
		void InitializeRevivePosition(const Vector2& revive_position);
		void SetRevivePosition(const Vector2& revive_position) { m_revive_position = revive_position; }
		// collision
		PlayerCollisionInfo& GetCollisionInfo() { return m_collision_info; }
		const PlayerCollisionInfo& GetCollisionInfo() const { return m_collision_info; }
		// bad bad
		Rect GetBoundingBox() {
			Rect bbox_world{ m_collision_bbox.GetSize(), m_collision_bbox.GetCenter() + m_transform.GetTransform2D().position };
			return bbox_world;
		}
		SpaceId GetInSpaceId() const { return m_status.in_space_id; }
		void TakeItem(bool sword);
		bool stuck{ false };
	private:
		static constexpr int NUM_SPRITES{ 2 };
		enum class PlayerTransferState
		{
			REVIVE,
			DIE,
			TAKE
		};
		enum class PlayerMoveState
		{
			DEFAULT,
			JUMP,
			DASH
		};
		enum class PlayerMoveStateSub
		{
			IDLE,
			WALK,
			RUN
		};
		enum class PlayerAttackState
		{
			IDLE,
			PRE_CHARGE,
			CHARGE,
			SLASH
		};
		struct PlayerInput
		{
			Vector2 movement{};
			bool desired_attack_release{ false };
			bool desired_attack_press{ false };
			bool desired_dash{ false };
			bool desired_jump{ false };
		};
		struct PlayerMoveConfig
		{
			// per frame
			float max_speed{ 12.0f };
			float max_acc{ 12.0f };
			float max_acc_air{ 2.0f };
			float gravity_up { 1.8f };
			float gravity_down{ 3.5f };
			float jump_height{ StageConfig::TILE_SIZE.y * 4.0f };
			float dash_speed{ 200.0f };
		};
		struct PlayerStatus
		{
			float hp{ 1.0f };
			int in_rain_countdown{ 0 };
			int in_attack_countdown{ 0 };
			SpaceId in_space_id{ SpaceId::SPACE_DEFAULT };
		};
		// stage =====
		//void SetStage(Stage* stage) { m_stage = stage;  }
		Stage* m_stage{ nullptr };
		// root states ================
		void UpdateActive();
		PlayerState m_state{ PlayerState::TRANSFER };

		// transfer states ========
		void EnterTransferState(PlayerTransferState transfer_state);
		void ExitTransferState();
		void UpdateTransfer();
		PlayerTransferState m_transfer_state{ PlayerTransferState::REVIVE };
		int m_transfer_countdown{ 0 };


		// active states ========
		// input ====
		void UpdateInput();
		PlayerInput m_input{};
		// player attack =====
		void UpdateAttack();
		PlayerAttackState m_attack_state{ PlayerAttackState::IDLE };
		int m_attack_state_countdown{ 0 };
		Sword* m_sword{ nullptr };
		bool m_sword_enabled{ false };
		PlayerAttackManager* m_attack_manager{ nullptr };
		TrailManager* m_trail_manager{ nullptr };
		const Sword::SwordConfig* m_sword_config{ nullptr };
		// player move ======
		// states
		void UpdateMove();
		void UpdateMoveDefault();
		void UpdateMoveJump();
		void UpdateMoveDash();
		// state change
		PlayerMoveStateSub GetNextMoveStateSub() const;
		void EnterMoveStateDefault(bool update = false);
		void EnterMoveStateJump();
		void EnterMoveStateDash();
		bool CheckEnterMoveStateJump();
		bool CheckEnterMoveStateDash();
		// move utils
		void MoveSpriteDirection();
		void MoveHorizontal();
		void ApplyGravity();
		PlayerMoveConfig m_move_config{};
		PlayerMoveState m_move_state{ PlayerMoveState::DEFAULT };
		PlayerMoveStateSub m_move_state_sub{ PlayerMoveStateSub::IDLE };
		// sub states
		int frames_since_desired_jump{ 1000 };
		int frames_since_on_ground{ 1000 };

		// sprite animation helpers =====
		void InitializeSprite();
		void SetSpriteAnimation(SpritePresetName preset);
		void SetSpriteFlipX(bool flip_x);
		SpriteState m_sprites[NUM_SPRITES];
		int m_sprite_id_character{ 0 };
		int m_sprite_id_effect{ 1 };

		bool m_dashing{ false };
		float m_dash_countdown{ 0 };

		float m_direction{ 1.0f };

		// player hurt, die, revive ========
		void CheckHurt();
		void UpdateInSpaceId();
		void HandleHurt(float damage);

		PlayerCollisionInfo m_collision_info{};
		PlayerStatus m_status{};
		Rect m_collision_bbox{};
		void CleanOnRevive();
		void CleanOnDying();
		Vector2 m_revive_position{};
	};
}