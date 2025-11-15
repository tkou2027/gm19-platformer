#include "enemy.h"

#include "component/render_sprites.h"
#include "global_context.h"
#include "physics/physics_system.h"
#include "resource/tile.h"
#include "platform/controller.h"
#include "stage.h"
#include "player.h"
#include "enemy_attack.h"
#include "trail_manager.h"
#include "platform/sound.h"

namespace foo
{
	EnemyShooter::EnemyShooter(SpaceId space_id, const Vector2& initial_position, bool key)
		: m_space_id(space_id), m_initial_position(initial_position), m_key(key)
	{
		// render
		TileManager* tile_manager = g_global_context.m_tile_manager;
		// body
		m_body_sprite_index = 0;
		m_sprites[m_body_sprite_index].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ENEMY_SHOOTER_BODY));
		m_sprites[m_body_sprite_index].space_id = m_space_id;
		// outline
		m_sprites[1].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ENEMY_SHOOTER_OUTLINE));
		m_eye_sprite_index = 2;
		m_sprites[m_eye_sprite_index].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ENEMY_SHOOTER_EYE));

		SetRenderComponent(new RenderSprites(m_sprites, NUM_SPRITES));

		if (key)
		{
			const float scale{ 1.6f };
			for (int i = 0; i < NUM_SPRITES; i++)
			{
				m_sprites[i].size = m_sprites[i].size * scale;
			}
			m_config.default_size *= scale;
			m_config.default_size_eye *= scale;
			m_config.default_size_body *= scale;
			m_config.aim_countdown = 30;
			m_config.search_radius = 900.0f;
		}

		// reference
		m_player = (Player*)GetNamedObject(ObjectName::PLAYER);
		m_attack_manager = (EnemyAttackManager*)GetNamedObject(ObjectName::ENEMY_ATTACK_MANAGER);
	}
	void EnemyShooter::Update()
	{
		switch (m_state)
		{
		case EnemyShooterState::IDLE:
		{
			UpdateIdle();
			break;
		}
		case EnemyShooterState::AIM:
		{
			UpdateAim();
			break;
		}
		case EnemyShooterState::ATTACK_DELAY:
		{
			UpdateCountdown(EnemyShooterState::ATTACK);
			break;
		}
		case EnemyShooterState::ATTACK:
		{
			UpdateCountdown(EnemyShooterState::COOLDOWN);
			break;
		}
		case EnemyShooterState::COOLDOWN:
		{
			UpdateCoolDown();
			break;
		}
		case EnemyShooterState::HURT:
		{
			UpdateCountdown(EnemyShooterState::DEAD);
			break;
		}
		}
		UpdateBodyAnimation();
		CheckHurt();
	}
	void EnemyShooter::SetActive()
	{
		EnterState(EnemyShooterState::IDLE);
		GameObject::SetActive();
	}
	void EnemyShooter::SetIdle()
	{
		if (m_attack)
		{
			m_attack->SetDead();
		}
		GameObject::SetIdle();
	}
	void EnemyShooter::SetRemoved()
	{
		if (m_attack)
		{
			m_attack->SetDead();
		}
		GameObject::SetRemoved();
	}
	void EnemyShooter::EnterState(EnemyShooterState state)
	{
		m_state_countdown = 0;
		switch (state)
		{
		case EnemyShooterState::IDLE:
		{
			m_render_component->SetActive(true);
			// m_sprites[m_eye_sprite_index].size = { 0.0f, 0.0f };
			m_sprites[m_eye_sprite_index].offset = { 0.0f, 0.0f };
			if (m_attack)
			{
				m_attack->SetDead();
				m_attack = nullptr;
			}
			m_transform.SetPosition(m_initial_position);
			break;
		}
		case EnemyShooterState::AIM:
		{
			m_state_countdown = m_config.aim_countdown;
			InitializeAttack();
			break;
		}
		case EnemyShooterState::ATTACK_DELAY:
		{
			// TODO: animation
			m_state_countdown = m_config.delay_countdown;
			break;
		}
		case EnemyShooterState::ATTACK:
		{
			m_state_countdown = m_config.attack_countdown;
			m_attack->SetActive();
			Controller* controller = g_global_context.m_controller;
			controller->SetControllerLeftVibration(1);
			controller->SetControllerRightVibration(1);
			break;
		}
		case EnemyShooterState::COOLDOWN:
		{
			m_sprites[m_eye_sprite_index].offset = { 0.0f, 0.0f };
			m_state_countdown = m_config.cooldown_countdown;
			break;
		}
		case EnemyShooterState::HURT:
		{
			DoHurtEffect();
			m_state_countdown = m_config.hurt_countdown;
			break;
		}
		case EnemyShooterState::DEAD:
		{
			m_render_component->SetActive(false);
			if (m_key)
			{
				auto stage = (Stage*)GetNamedObject(ObjectName::STAGE);
				stage->EnterExit();
			}
			break;
		}
		}
		m_state = state;
	}

	void EnemyShooter::UpdateIdle()
	{
		if (GetTarget())
		{
			EnterState(EnemyShooterState::AIM);
		}
		UpdateAimAnimation();
	}

	void EnemyShooter::UpdateAim()
	{
		if (!GetTarget())
		{
			EnterState(EnemyShooterState::IDLE);
			return;
		}
		if (m_state_countdown == 0)
		{
			m_sprites[m_eye_sprite_index].size.x = m_config.default_size_eye;
			m_sprites[m_eye_sprite_index].size.y = m_config.default_size_eye;
			EnterState(EnemyShooterState::ATTACK_DELAY);
			return;
		}
		// animation
		if (m_sprites[m_eye_sprite_index].size.x < m_config.default_size_eye)
		{
			float delta_size = Math::Min(
				m_config.default_size_eye - m_sprites[m_eye_sprite_index].size.x,
				Math::Max(10.0f, (m_config.default_size_eye - m_sprites[m_eye_sprite_index].size.x) * 0.5f)
			);
			m_sprites[m_eye_sprite_index].size.x += delta_size;
			m_sprites[m_eye_sprite_index].size.y += delta_size;
		}
		UpdateAimAnimation();
		UpdateAttackAim();
		--m_state_countdown;
	}

	void EnemyShooter::UpdateCoolDown()
	{
		if (m_state_countdown == 0)
		{
			EnterState(EnemyShooterState::IDLE);
			return;
		}
		if (m_sprites[m_eye_sprite_index].size.x > 0.0f)
		{
			float delta_size = Math::Min(
				m_sprites[m_eye_sprite_index].size.x,
				Math::Max(10.0f, (m_sprites[m_eye_sprite_index].size.x) * 0.5f)
			);
			m_sprites[m_eye_sprite_index].size.x -= delta_size;
			m_sprites[m_eye_sprite_index].size.y -= delta_size;
		}
		--m_state_countdown;
	}

	void EnemyShooter::UpdateAimAnimation()
	{
		const Vector2& player_pos = m_player->GetPlayerPositionCenter();
		const Vector2& my_pos = m_transform.GetTransform2D().position;
		Vector2 direction = player_pos - my_pos;
		direction.Normalize();
		m_sprites[m_eye_sprite_index].offset = direction * 10.0f;
		m_sprites[m_body_sprite_index].offset = direction * -4.0f;
	}

	void EnemyShooter::UpdateBodyAnimation()
	{
		float t = m_yoyo.GetT();
		float factor = 1.0f;
		if (m_state == EnemyShooterState::ATTACK_DELAY)
		{
			t = t * 4.0f;
			t -= floorf(t);
			factor = 4.0f;
		}
		else if (m_state == EnemyShooterState::ATTACK)
		{
			t = 1.0f;
			factor = 2.0f;
		}
		else if (m_state == EnemyShooterState::HURT)
		{
			t = t * 8.0f;
			t -= floorf(t);
			factor = 0.0f;
		}
		t = Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_IN_OUT_BACK, t);
		float body_size = m_config.default_size_body
			+ t * factor * m_config.float_size_body;
		m_sprites[m_body_sprite_index].size = { body_size, body_size };
		if (m_state == EnemyShooterState::HURT)
		{
			m_sprites[m_body_sprite_index].offset = { t * 8.0f , 0.0f };
		}
		if (m_state == EnemyShooterState::IDLE)
		{
			// float
			float velocity = (m_yoyo.GetT() - 0.5f) * 2.0f * m_config.floating_range; // per frame
			m_transform.SetPosition(m_initial_position + Vector2{ 0.0f, 1.0f } * velocity);
		}
		m_yoyo.Update();
	}

	void EnemyShooter::UpdateCountdown(EnemyShooterState next_state)
	{
		if (m_state_countdown == 0)
		{
			EnterState(next_state);
		}
		--m_state_countdown;
	}

	void EnemyShooter::CheckHurt()
	{
		if (m_state == EnemyShooterState::HURT || m_state == EnemyShooterState::DEAD)
		{
			return;
		}
		Rect box{ { m_config.default_size, m_config.default_size }, m_transform.GetTransform2D().position };
		if (g_global_context.m_physics_system->HandleOverLap(box, CollisionTestLayer::ENEMY, this, CollisionTriggerLayer::PLAYER_ATTACK))
		{
			EnterState(EnemyShooterState::HURT);
		}
	}

	bool EnemyShooter::GetTarget()
	{
		const Vector2& my_pos = m_transform.GetTransform2D().position;
		if (!m_player->IfPlayerInDistance(my_pos, m_config.search_radius))
		{
			return false;
		}
		if (m_player->GetInSpaceId() == m_space_id)
		{
			return false;
		}
		return true;
	}
	void EnemyShooter::InitializeAttack()
	{
		const Vector2& player_pos = m_player->GetPlayerPositionCenter();
		const Vector2& my_pos = m_transform.GetTransform2D().position;
		EnemyAttackRay* ray = new EnemyAttackRay(my_pos, player_pos, m_space_id);
		m_attack = ray;
		m_attack_manager->Add(ray);
	}

	void EnemyShooter::UpdateAttackAim()
	{
		const Vector2& player_pos = m_player->GetPlayerPositionCenter();
		const Vector2& my_pos = m_transform.GetTransform2D().position;
		m_attack->SetShape(my_pos, player_pos);
	}
	void EnemyShooter::DoHurtEffect()
	{
		m_sprites[m_eye_sprite_index].size = Vector2{ m_config.default_size, m_config.default_size };
		m_sprites[m_eye_sprite_index].offset = { 0.0f, 0.0f };
		m_sprites[m_body_sprite_index].offset = { 0.0f, 0.0f };
		if (m_attack)
		{
			m_attack->SetDead();
			m_attack = nullptr;
		}

		SetHitStop(m_key ? 30 : 4);
		Controller* controller = g_global_context.m_controller;
		controller->SetControllerLeftVibration(1);
		controller->SetControllerRightVibration(1);

		if (m_key)
		{
			g_global_context.m_sound->PlaySe(SeName::CLICK);

			TrailManager* trail_manager = (TrailManager*)GetNamedObject(ObjectName::TRAIL_MANAGER);
			trail_manager->ClearScope(-2);
		}
	}
}