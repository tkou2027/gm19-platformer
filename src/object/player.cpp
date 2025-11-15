#include "player.h"

// components
#include "component/render_sprites.h"
#include "component/render_after_image.h"
#include "component/physics_actor.h"
#include "physics/colliders.h"
#include "sword.h"
#include "particle_system.h"
#include "player_attack.h"
#include "trail_manager.h"
#include "stage.h"
// systems
#include "global_context.h"
#include "platform/controller.h";
#include "platform/timer.h"
#include "render/render_system.h"
#include "physics/physics_system.h"

#include "util/random.h"

// #include "render/texture.h"

namespace foo
{
	Player::Player()
	{
		// render
		// TODO: init transform
		RenderComponentList* render_components = new RenderComponentList();

		// render sprite
		// SpriteConfigInput sprite_config{};
		InitializeSprite();
		RenderSprites* render_sprites = new RenderSprites(m_sprites, NUM_SPRITES);
		render_components->Add(render_sprites);

		// render after images
		//RenderAfterImage::AfterimageConfig config{};
		//config.render_sprite = render_sprite;
		//render_components->Add(new RenderAfterImage(config));

		SetRenderComponent(render_components);

		// physics
		m_collision_bbox = Rect{ Vector2{ 46, 94 }, Vector2{ 0, -47 } };
		SetPhysicsComponent(new PhysicsActor(
			new BoxCollider(m_collision_bbox.GetSize(), m_collision_bbox.GetCenter()),
			PhysicsActorLayer::PLAYER
		));
		// m_transform.SetPosition({ 0, 0, 0 });

		// set named object
		m_name = ObjectName::PLAYER;
		SetNamedObject(m_name, this);

		// reference
		m_attack_manager = (PlayerAttackManager*)GetNamedObject(ObjectName::PLAYER_ATTACK_MANAGER);
		m_trail_manager = (TrailManager*)GetNamedObject(ObjectName::TRAIL_MANAGER);

		// init
		EnterTransferState(PlayerTransferState::REVIVE);

		// debug
		// m_sword_enabled = true;
	}

	// root states ========
	void Player::Update()
	{
		stuck = false;
		switch (m_state)
		{
		case PlayerState::TRANSFER:
		{
			UpdateTransfer();
			break;
		}
		case PlayerState::ACTIVE:
		{
			UpdateActive();
			break;
		}
		}
	}

	bool Player::IfPlayerInDistance(const Vector2& from, float distance)
	{
		if (m_state != PlayerState::ACTIVE)
		{
			return false;
		}
		const Vector2& my_pos = m_transform.GetTransform2D().position + m_collision_bbox.GetCenter();
		float dist_squared = (my_pos - from).LengthSquared();
		return distance < 0.0f || dist_squared < distance * distance;
	}

	Vector2 Player::GetPlayerPositionCenter()
	{
		Vector2 pos = m_transform.GetTransform2D().position;
		return pos + m_collision_bbox.GetCenter();
	}

	void Player::InitializeRevivePosition(const Vector2& revive_position)
	{
		m_revive_position = revive_position;
		EnterTransferState(PlayerTransferState::REVIVE);
	}

	void Player::TakeItem(bool sword)
	{
		if (sword)
		{
			m_sword_enabled = true;
		}
		SetHitStop(10);
		// EnterTransferState(PlayerTransferState::TAKE);
	}

	// transfer states ========
	void Player::EnterTransferState(PlayerTransferState transfer_state)
	{
		m_state = PlayerState::TRANSFER;
		m_physics_component->SetVelocity({ 0.0f, 0.0f });
		m_transfer_state = transfer_state;
		switch (m_transfer_state)
		{
		case PlayerTransferState::REVIVE:
		{
			m_transfer_countdown = 30;
			// TODO: reset room
			m_transform.SetPosition(m_revive_position);
			SetSpriteAnimation(SpritePresetName::PLAYER_REVIVE);
			CleanOnRevive();
			break;
		}
		case PlayerTransferState::DIE:
		{
			m_transfer_countdown = 60;
			SetSpriteAnimation(SpritePresetName::PLAYER_DIE);
			CleanOnDying();
			break;
		}
		case PlayerTransferState::TAKE:
		{
			m_transfer_countdown = 60;
			SetSpriteAnimation(SpritePresetName::PLAYER_IDLE);
			break;
		}
		}
	}

	void Player::ExitTransferState()
	{
		switch (m_transfer_state)
		{
		case PlayerTransferState::REVIVE:
		case PlayerTransferState::TAKE:
		{
			// TODO
			m_state = PlayerState::ACTIVE;
			// SetSpriteAnimation(SpritePresetName::PLAYER_IDLE);
			EnterMoveStateDefault();
			break;
		}
		case PlayerTransferState::DIE:
		{
			EnterTransferState(PlayerTransferState::REVIVE);
			break;
		}
		}
	}

	void Player::UpdateTransfer()
	{
		if (m_transfer_countdown == 0)
		{
			ExitTransferState();
		}
		m_transfer_countdown--;
		// TODO
	}
	// PlayerUpdate =======

	void Player::UpdateActive()
	{
		UpdateInput();
		UpdateAttack();
		UpdateMove();
		CheckHurt();
	}

	void Player::InitializeSprite()
	{
		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprites[m_sprite_id_character].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::PLAYER_IDLE));
		m_sprites[m_sprite_id_effect].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::PLAYER_EFFECT_NONE));
	
		// space color
		UpdateInSpaceId();
	}

	void Player::SetSpriteAnimation(SpritePresetName preset)
	{
		// TODO: preloading
		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprites[m_sprite_id_character].SetConfigInput(tile_manager->LoadSpriteConfig(preset));

		SpritePresetName effect_preset = SpritePresetName::PLAYER_ATTACK_SLASH_EFFECT;
		switch (preset)
		{
		case SpritePresetName::PLAYER_ATTACK_SLASH:
		{
			effect_preset = SpritePresetName::PLAYER_ATTACK_SLASH_EFFECT;
			break;
		}
		case SpritePresetName::PLAYER_ATTACK_CHARGE_SLASH:
		{
			effect_preset = SpritePresetName::PLAYER_ATTACK_CHARGE_SLASH_EFFECT;
			break;
		}
		default:
		{
			effect_preset = SpritePresetName::PLAYER_EFFECT_NONE;
			break;
		}
		}
		m_sprites[m_sprite_id_effect].SetConfigInput(tile_manager->LoadSpriteConfig(effect_preset));
	}

	void Player::SetSpriteFlipX(bool flip_x)
	{
		m_sprites[m_sprite_id_character].flip_x = flip_x;
		m_sprites[m_sprite_id_effect].flip_x = flip_x;
	}

	// gamepad input
	void Player::UpdateInput()
	{
		const Controller* controller = g_global_context.m_controller;
		m_input.movement = controller->GetControllerLeftStick();
		m_input.movement.ClampMagnitude(1.0f);
		if (abs(m_input.movement.x) < 0.01f)
		{
			m_input.movement.x = 0.0f;
		}

		m_input.desired_attack_release = false;
		if (controller->GetControllerTrigger(Button::BUTTON_B))
		{
			m_input.desired_attack_press = true;
		}
		// since there is not release event...
		if (!controller->GetControllerPress(Button::BUTTON_B))
		{
			if (m_input.desired_attack_press)
			{
				m_input.desired_attack_release = true;
				m_input.desired_attack_press = false;
			}
		}
		// m_input.desired_attack = controller->GetControllerTrigger(Button::BUTTON_B);

		m_input.desired_jump = controller->GetControllerTrigger(Button::BUTTON_A);

		//m_input.desired_dash = controller->GetControllerPress(Button::BUTTON_A);
	}

	void Player::UpdateAttack()
	{
		if (!m_sword || !m_sword_enabled)
		{
			return;
		}

		switch (m_attack_state)
		{
		case PlayerAttackState::IDLE:
		{
			if (m_input.desired_attack_press)
			{
				m_attack_state = PlayerAttackState::PRE_CHARGE;
				SetSpriteAnimation(SpritePresetName::PLAYER_ATTACK_PRE_CHARGE);
				m_sword->StartPreCharge();
				m_attack_state_countdown = m_sword_config->pre_charge_countdown;
			}
			break;
		}
		case PlayerAttackState::PRE_CHARGE:
		{
			if (m_attack_state_countdown == 0)
			{
				// enter charge
				m_attack_state = PlayerAttackState::CHARGE;
				SetSpriteAnimation(SpritePresetName::PLAYER_ATTACK_CHARGE);
				m_sword->StartCharge();
				break;
			}
			if (m_input.desired_attack_release) // release
			{
				// enter slash
				m_attack_state = PlayerAttackState::SLASH;
				SetSpriteAnimation(SpritePresetName::PLAYER_ATTACK_SLASH);
				m_sword->StartSlash();
				m_attack_state_countdown = 20; // TODO
				break;
			}
			m_attack_state_countdown--;
			break;
		}
		case PlayerAttackState::SLASH:
		{
			if (m_attack_state_countdown == 0)
			{
				m_attack_state = PlayerAttackState::IDLE;
				// SetSpriteAnimation(SpritePresetName::PLAYER_IDLE);
				EnterMoveStateDefault();
			}
			m_attack_state_countdown--;
			break;
		}
		case PlayerAttackState::CHARGE:
		{
			if (m_input.desired_attack_release) // release
			{
				// enter charged attack
				m_attack_state = PlayerAttackState::SLASH; // ?
				SetSpriteAnimation(SpritePresetName::PLAYER_ATTACK_CHARGE_SLASH);
				m_sword->StartChargeSlash();
				m_attack_state_countdown = 20; // TODO
			}
			break;
		}
		}
	}

	// states
	void Player::UpdateMove()
	{
		if (m_sword)
		{
			if (m_sword->IfLockMovement())
			{
				if (!m_sword->IfLockRotation())
				{
					MoveSpriteDirection();
				}
				m_physics_component->SetVelocityX(0);
				ApplyGravity();
				return;
			}
		}
		switch (m_move_state)
		{
		case PlayerMoveState::DEFAULT:
		{
			UpdateMoveDefault();
			break;
		}
		case PlayerMoveState::JUMP:
		{
			UpdateMoveJump();
			break;
		}
		case PlayerMoveState::DASH:
		{
			UpdateMoveDash();
			break;
		}
		}
	}

	void Player::UpdateMoveDefault()
	{
		// state change
		if (CheckEnterMoveStateDash() || CheckEnterMoveStateJump())
		{
			return;
		}
		// sub state change
		PlayerMoveStateSub next_state_sub = GetNextMoveStateSub();
		if (m_move_state_sub != next_state_sub)
		{
			if (next_state_sub == PlayerMoveStateSub::IDLE)
			{
				SetSpriteAnimation(SpritePresetName::PLAYER_IDLE);
			}
			else
			{
				SetSpriteAnimation(SpritePresetName::PLAYER_MOVE);
			}
			m_move_state_sub = next_state_sub;
		}
		// update
		MoveSpriteDirection();
		MoveHorizontal();
		ApplyGravity();
	}

	void Player::UpdateMoveJump()
	{
		// state change
		if (CheckEnterMoveStateDash())
		{
			return;
		}
		if (((PhysicsActor*)m_physics_component)->GetOnGround())
		{
			EnterMoveStateDefault();
			
			// fall effect
			// vibration
			Controller* controller = g_global_context.m_controller;
			controller->SetControllerLeftVibration(1);
			controller->SetControllerRightVibration(1);
			// particle
			{
				ParticleSystem* particle_system = (ParticleSystem*)GetNamedObject(ObjectName::PARTICLE_SYSTEM);
				float i = -m_direction;
				Particle p{};
				p.size = Vector2{ 15.0, 15.0f };
				p.position = m_transform.GetTransform2D().position;
				p.position.x += 20.0f * i;
				p.velocity = Vector2{ 0.5f * i, -2.0f };
				p.space_id = SpaceId::SPACE_A; // TODO
				p.life = 10;
				particle_system->AddParticle(p);
			}
			// fall through, no return
		}

		// update
		MoveSpriteDirection();
		MoveHorizontal();
		ApplyGravity();
	}

	void Player::UpdateMoveDash()
	{
		// trigger_dash = false;
		float delta_time = g_global_context.m_timer->GetFixedDeltaTime();
		m_dash_countdown -= delta_time;

		// slow down
		Vector2 velocity = m_physics_component->GetVelocity();
		velocity.x = velocity.x > 0 ? velocity.x - 100 : velocity.x + 100;
		m_physics_component->SetVelocity(velocity);
		if (m_dash_countdown <= 0 || m_physics_component->GetVelocity().x == 0.0f)
		{
			RenderComponentList* render_components = (RenderComponentList*)m_render_component;
			RenderAfterImage* render_after_image = (RenderAfterImage*)(*render_components)[1];
			render_after_image->SetActive(false);
			m_dash_countdown = 0;
			EnterMoveStateDefault();
		}
	}

	Player::PlayerMoveStateSub Player::GetNextMoveStateSub() const
	{
		if (abs(m_input.movement.x) < 0.04f)
		{
			return PlayerMoveStateSub::IDLE;
		}
		if (abs(m_input.movement.x) < 0.5f)
		{
			return PlayerMoveStateSub::WALK;
		}
		return PlayerMoveStateSub::RUN;
	}

	void Player::EnterMoveStateDefault(bool update)
	{
		m_move_state = PlayerMoveState::DEFAULT;

		// set sub state
		PlayerMoveStateSub state_sub = GetNextMoveStateSub();
		if (update && m_move_state_sub == state_sub)
		{
			// nothing new
			return;
		}
		switch (state_sub)
		{
		case  PlayerMoveStateSub::IDLE:
		{
			SetSpriteAnimation(SpritePresetName::PLAYER_IDLE);
			break;
		}
		case PlayerMoveStateSub::WALK:
		case PlayerMoveStateSub::RUN:
		{
			SetSpriteAnimation(SpritePresetName::PLAYER_MOVE);
			// TODO
			break;
		}
		}
	}

	void Player::EnterMoveStateJump()
	{
		m_move_state = PlayerMoveState::JUMP;
		float next_velocity_y = -sqrtf(m_move_config.gravity_up * 2 * m_move_config.jump_height);
		m_physics_component->SetVelocityY(next_velocity_y);
	}

	void Player::EnterMoveStateDash()
	{
		m_move_state = PlayerMoveState::DASH;
		//RenderComponentList* render_components = (RenderComponentList*)m_render_component;
		//RenderAfterImage* render_after_image = (RenderAfterImage *)(*render_components)[1];
		//RenderSprite* render_sprite = (RenderSprite*)(*render_components)[0];
		//render_sprite->SetFlipX(m_input.movement.x < 0);

		SetSpriteFlipX(m_input.movement.x < 0);
		// m_sword->SetDash(m_input.movement.x);

		// enter dash
		//render_after_image->SetActive(true);
		Vector2 velocity{ 3000.0f * (m_input.movement.x > 0 ? 1.0f : -1.0f), 0.0f };
		m_physics_component->SetVelocity(velocity);
		m_dash_countdown = 0.5f;

		// trigger_dash = true;
	}

	bool Player::CheckEnterMoveStateJump()
	{
		if (m_input.desired_jump && ((PhysicsActor*)m_physics_component)->GetOnGround())
		{
			// TODO
			EnterMoveStateJump();
			return true;
		}
		return false;
	}

	bool Player::CheckEnterMoveStateDash()
	{
		if (m_input.desired_dash)
		{
			EnterMoveStateDash();
			return false;
		}
		return false;
	}

	void Player::MoveSpriteDirection()
	{
		if (abs(m_input.movement.x) >= 0.1f)
		{
			SetSpriteFlipX(m_input.movement.x < 0.0f);
			m_direction = m_input.movement.x < 0.0f ? -1.0f : 1.0f;
			if (m_sword)
			{
				m_sword->SetDirection(m_direction);
			}
		}
	}
	void Player::MoveHorizontal()
	{
		// speed
		float acc = m_move_state == PlayerMoveState::JUMP ? m_move_config.max_acc_air : m_move_config.max_acc;
		float target_velocity_x = m_move_config.max_speed * m_input.movement.x;
		float next_velocity_x = Math::MoveTowards(m_physics_component->GetVelocity().x, target_velocity_x, acc);
		m_physics_component->SetVelocityX(next_velocity_x);
	}

	void Player::ApplyGravity()
	{
		float curr_velocity_y = m_physics_component->GetVelocity().y;
		float next_velocity_y = curr_velocity_y + (curr_velocity_y < 0 ? m_move_config.gravity_up : m_move_config.gravity_down);
		m_physics_component->SetVelocityY(next_velocity_y);
	}

	// Player hurt =============================
	void Player::CheckHurt()
	{
		UpdateInSpaceId();
		// enemy attack
		// muteki
		if (m_attack_state == PlayerAttackState::SLASH)
		{
			return;
		}
		// 
		m_collision_info = {}; // reset collision info
		// TODO
		Rect bbox_world{ m_collision_bbox.GetSize(), m_collision_bbox.GetCenter() + m_transform.GetTransform2D().position };
		if (g_global_context.m_physics_system->HandleOverLap(bbox_world, CollisionTestLayer::PLAYER, this, CollisionTriggerLayer::ENEMY_ATTACK))
		{
			// FOO_LOG("agg");
			if (m_collision_info.hurt)
			{
				m_status.in_attack_countdown++;
				if (m_status.in_attack_countdown >= 4)
				{
					HandleHurt(1.0f); // TODO
				}
			}
			if (m_collision_info.in_rain)
			{
				m_status.in_rain_countdown++;
				// Hurt
				if (g_random.NextFloat() < 0.5)
				{
					m_sprites[m_sprite_id_character].color = g_space_config.GetSpaceProp(SpaceId::SPACE_A).color; // TODO

					if (g_random.NextFloat() < 0.5)
					{
						ParticleSystem* particle_system = (ParticleSystem*)GetNamedObject(ObjectName::PARTICLE_SYSTEM);
						float i = -m_direction;
						Particle p{};
						p.size = Vector2{ 15.0, 15.0f };
						p.position = m_transform.GetTransform2D().position;
						p.position.x += g_random.NextFloat() * 40.0f - 20.0f;
						p.velocity = Vector2{ g_random.NextFloatRange(-2.0f, 2.0f), -2.0f * g_random.NextFloat() };
						p.space_id = SpaceId::SPACE_A; // TODO
						p.life = 4;
						particle_system->AddParticle(p);
					}
					// vibe
					//Controller* controller = g_global_context.m_controller;
					//controller->SetControllerLeftVibration(1);
					//controller->SetControllerRightVibration(1);
				}
				if (m_status.in_rain_countdown >= 60)
				{
					HandleHurt(1.0f); // TODO
				}
			}
			else
			{
				m_status.in_rain_countdown = 0;
			}
		}
	}

	void Player::UpdateInSpaceId()
	{

		Rect bbox_world{ m_collision_bbox.GetSize(), m_collision_bbox.GetCenter() + m_transform.GetTransform2D().position };
		int max_cnt{ 0 };
		SpaceId max_space{ SpaceId::SPACE_DEFAULT };
		int cnt[(int)SpaceId::SPACE_MAX]{ 0, 0, 0 };
		for (int i = 0; i < 5; i++)
		{
			const Vector2 pos = bbox_world.GetCorner(i);
			SpaceId space_id = g_global_context.m_physics_system->GetSpaceId(pos);
			int space_cnt = ++cnt[(int)space_id];
			if (space_cnt > max_cnt)
			{
				max_space = space_id;
				max_cnt = space_cnt;
			}
		}
		m_status.in_space_id = max_space;


		if (stuck)
		{
			max_space = SpaceId::SPACE_A;
		}

		// will be overwrited when hurt
		m_sprites[m_sprite_id_character].color = max_space == SpaceId::SPACE_A ?
			Vector4(1.0f, 1.0f, 1.0f, 1.0f) : Vector4(0.0f, 0.0f, 0.0f, 1.0f); // Vector4(0.200f, 0.224, 0.255f, 1.0f);
	}

	void Player::HandleHurt(float damage)
	{
		m_status.hp -= damage;
		m_sprites[m_sprite_id_character].color = g_space_config.GetSpaceProp(SpaceId::SPACE_A).color; // TODO
		SetHitStop(4);
		// vibe
		Controller* controller = g_global_context.m_controller;
		controller->SetControllerLeftVibration(1);
		controller->SetControllerRightVibration(1);
		if (m_status.hp <= 0.0f)
		{
			m_status.hp = 0.0f;
			SetHitStop(8);
			EnterTransferState(PlayerTransferState::DIE);
		}
	}
	void Player::CleanOnRevive()
	{
		// input
		m_input = {};
		// attack
		m_attack_state = PlayerAttackState::IDLE;
		m_attack_state_countdown = 0;
		// move
		m_move_state = PlayerMoveState::DEFAULT;
		m_move_state_sub = PlayerMoveStateSub::IDLE;
		// hp...
		m_status = PlayerStatus{};
		// ???
		UpdateInSpaceId();
		// die animation over
		if (m_stage)
		{
			m_stage->EnterFadeReset();
		}
	}

	void Player::CleanOnDying()
	{
		// die animation start
		if (m_sword)
		{
			m_sword->StartIdle();
		}
		m_trail_manager->ClearScope(-1);
		m_attack_manager->Clear();
	}
}