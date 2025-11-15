#include "sword.h"

#include "player.h"
#include "blade.h"
#include "math/math.h"
#include "trail_manager.h"
#include "player_attack.h"
#include "global_context.h"
#include "resource/tile.h"
#include "component/render_sprites.h"
#include "platform/sound.h"
#include "platform/controller.h"

namespace foo
{
	Sword::Sword()
	{
		m_blade = new Blade(m_space_id, m_blade_size);
		m_transform.SetRotation(Math::HALF_PI);
		AddObject(m_blade, this);
		//TileManager* tile_manager = g_global_context.m_tile_manager;
		//m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_SWORD_HANDLE));
		//SetRenderComponent(new RenderSprites(m_sprites, NUM_SPRITES));
	}

	void Sword::Update()
	{
		switch (m_state)
		{
		case SwordState::IDLE:
		case SwordState::PRE_CHARGE:
		{
			// TODO
			break;
		}
		case SwordState::SLASH:
		{
			if (m_state_countdown == m_config.slash_countdown - 8)
			{
				m_transform.SetRotation(Math::THREE_HALVES_PI + m_direction * Math::HALF_PI * 0.8f);
			}
			else if (m_state_countdown == 0)
			{
				m_blade->SetSize({ 0.0f, m_blade_size.y });
				m_state = SwordState::IDLE; // TODO
				break;
			}
			m_state_countdown--;
			break;
		}
		case SwordState::CHARGE:
		{
			float t = (m_config.charge_countdown - m_state_countdown) / (float)m_config.charge_countdown;
			float charge_len = 30.0f + 20.0f * t;
			m_blade->SetSize({ m_blade_size.x + charge_len, m_blade_size.y });
			if (m_state_countdown > 0)
			{
				m_state_countdown--;
			}
			break;
		}
		case SwordState::CHARGE_SLASH:
		{
			if (m_state_countdown == m_config.slash_countdown - 1)
			{
				m_transform.SetRotation(Math::THREE_HALVES_PI + m_direction * Math::HALF_PI * 1.1f);
				DoChargeSlash(); // collision
			}
			else if (m_state_countdown == 0)
			{
				m_blade->SetSize({ 0.0f, m_blade_size.y });
				m_state = SwordState::IDLE; // TODO
				break;
			}
			m_state_countdown--;
			break;
		}
		}
	}

	// events
	void Sword::SetPlayer(Player* player)
	{
		m_player = player;
		m_player->SetSword(this);
		m_player->GetTransform().AddChild(&m_transform);
		m_transform.SetPosition({ 0.0f, -40.0f });
		//m_transform.SetRotation(Math::DegToRad(-30.0f));
		m_blade->SetSize({ 0.0f, m_blade_size.y });
	}

	void Sword::StartPreCharge()
	{
		m_blade->SetSize({ m_blade_size.x, m_blade_size.y }); // TODO
		m_transform.SetRotation(Math::THREE_HALVES_PI - m_direction * Math::HALF_PI * 0.95f);
		m_state_countdown = m_config.charge_countdown;
		m_state = SwordState::PRE_CHARGE;
	}

	void Sword::StartCharge()
	{
		m_blade->SetSize({ m_blade_size.x, m_blade_size.y }); // TODO
		m_transform.SetRotation(Math::THREE_HALVES_PI - m_direction * Math::HALF_PI);
		m_state_countdown = m_config.charge_countdown;
		m_state = SwordState::CHARGE;
	}

	void Sword::StartSlash()
	{
		m_transform.SetRotation(Math::THREE_HALVES_PI + m_direction * Math::HALF_PI * 0.9f);
		m_state_countdown = m_config.slash_countdown;
		m_state = SwordState::SLASH;
		DoSlash(); // collision
	}

	void Sword::StartChargeSlash()
	{
		m_transform.SetRotation(Math::THREE_HALVES_PI - m_direction * Math::HALF_PI * 1.1f);
		m_state_countdown = m_config.charge_slash_countdown;
		m_state = SwordState::CHARGE_SLASH;
	}

	void Sword::StartIdle()
	{
		m_blade->SetSize({ 0.0f, m_blade_size.y });
		m_state = SwordState::IDLE;
	}

	void Sword::SetDirection(float direction)
	{
		m_direction = Math::Sign(direction);
		if (!IfLockRotation())
		{
			m_transform.SetRotation(Math::THREE_HALVES_PI - m_direction * Math::HALF_PI);
		}
	}

	bool Sword::IfLockMovement() const
	{
		return m_state != SwordState::IDLE;
	}

	bool Sword::IfLockRotation() const
	{
		return m_state == SwordState::SLASH || m_state == SwordState::CHARGE_SLASH;
	}

	void Sword::DoSlash()
	{
		float radius = m_blade->GetSize().x;

		TrailObjectCircle::TrailObjectCircleConfig trail_config{};
		trail_config.center = m_transform.GetTransform2D().position;
		trail_config.radius = radius;
		trail_config.expand_radius = 10.0f;
		trail_config.expand_frames = 4;
		trail_config.yoyo_frames = 20;
		TrailObjectCircle* trail = new TrailObjectCircle(m_space_id, -1, trail_config);
		//m_trail_manager->Create(Shape::ShapeType::PIE);
		trail->SetSpaceId(m_space_id);
		TrailManager* trail_manager = (TrailManager*)GetNamedObject(ObjectName::TRAIL_MANAGER);
		trail_manager->Add(trail);

		PlayerAttackManager* player_attacks = (PlayerAttackManager*)GetNamedObject(ObjectName::PLAYER_ATTACK_MANAGER);
		player_attacks->Add(new PlayerAttack({
			m_transform.GetTransform2D().position,
			radius + 8.0f
		}));

		// vibe
		Controller* controller = g_global_context.m_controller;
		controller->SetControllerLeftVibration(1);
		controller->SetControllerRightVibration(1);

		g_global_context.m_sound->PlaySe(SeName::HIT);

		Camera& camera = g_global_context.m_render_system->GetCamera();
		camera.TriggerShake({ 0.0f, 4.0f });

		m_state_countdown = m_config.slash_countdown;
		m_state = SwordState::SLASH;
	}

	void Sword::DoChargeSlash()
	{
		// m_transform.SetRotation(Math::THREE_HALVES_PI + m_direction * Math::HALF_PI * 1.1f);
		// TODO: generate particle and attack
		// m_blade->StartSwing();

		float radius = m_blade->GetSize().x;

		TrailObjectCircle::TrailObjectCircleConfig trail_config{};
		Vector2 center = m_transform.GetTransform2D().position;
		center.y -= 20.0f;
		trail_config.center = center;
		trail_config.radius = radius;
		trail_config.expand_radius = 30.0f;
		trail_config.expand_frames = 8;
		trail_config.yoyo_frames = 60;
		TrailObjectCircle* trail = new TrailObjectCircle(m_space_id, -1, trail_config);
		//m_trail_manager->Create(Shape::ShapeType::PIE);
		trail->SetSpaceId(m_space_id);
		TrailManager* trail_manager = (TrailManager*)GetNamedObject(ObjectName::TRAIL_MANAGER);
		trail_manager->Add(trail);

		PlayerAttackManager* player_attacks = (PlayerAttackManager*)GetNamedObject(ObjectName::PLAYER_ATTACK_MANAGER);
		player_attacks->Add(new PlayerAttack({
			m_transform.GetTransform2D().position,
			radius + 15.0f
		}));

		Controller* controller = g_global_context.m_controller;
		controller->SetControllerLeftVibration(2);
		controller->SetControllerRightVibration(2);

		g_global_context.m_sound->PlaySe(SeName::HIT);

		Camera& camera = g_global_context.m_render_system->GetCamera();
		camera.TriggerShake({ 0.0f, 4.0f });
	}

	//// states
	//void Sword::SwordStateSlash::Enter()
	//{
	//	// enter anticipation
	//	m_state = SwordSlashState::ANTICIPATION;
	//	m_direction = m_sword.GetDirection();
	//	// set transform
	//	Transform trans = m_sword.GetTransform();
	//	trans.SetRotation(Math::THREE_HALVES_PI - m_direction * Math::HALF_PI);
	//	m_sword.SetTransform(trans);
	//	// set count down
	//	m_frame_count = 40;
	//	m_blade->SetSize({ 0.0f, 40.0f });
	//}
	//void Sword::SwordStateSlash::Update()
	//{
	//	switch (m_state)
	//	{
	//	case SwordSlashState::IDLE: break;
	//	case SwordSlashState::ANTICIPATION:
	//	{
	//		m_blade->SetSize({ (5.0f - m_frame_count / 8) * 40.0f, 20.0f });
	//		if (m_frame_count <= 0)
	//		{
	//			// enter attack
	//			m_state = SwordSlashState::ATTACK;
	//			//m_rotation_tween.Init(&m_sword, 1.0f, Tween::TWEEN_FUNCTION::EASE_IN_QUAD)
	//			//	.SetTweenTo(Tween::TWEEN_TYPE::ROTATION, Math::THREE_HALVES_PI + m_direction * Math::TWO_PI);
	//			// set transform
	//			Transform trans = m_sword.GetTransform();
	//			trans.SetRotation(Math::THREE_HALVES_PI + m_direction * Math::HALF_PI * 1.1f);
	//			m_sword.SetTransform(trans);
	//			m_blade->StartSwing();

	//			// m_trail_object = m_trail_manager->Create(Shape::ShapeType::PIE);


	//			// hit stop
	//			m_frame_count = 8;
	//		}
	//		else
	//		{
	//			m_frame_count--;
	//		}
	//		break;
	//	}
	//	case SwordSlashState::ATTACK:
	//	{
	//		// TODO
	//		//const TrailObject* trail_object = m_blade->GetTrailObject();
	//		//if (trail_object)
	//		//{
	//		//	const TrailShape& trail_shape = trail_object->GetShape();
	//		//	m_sword.m_trail_shape = trail_shape;
	//		//}

	//		if (m_frame_count <= 0)
	//		{
	//			// TODO: test test cut space
	//			//Space* space = (Space*)m_sword.GetNamedObject(ObjectName::SPACE);
	//			//Transform2D trans = m_sword.GetTransform().GetTransform2D();
	//			//space->Cut(trans.position, m_direction > 0.0f ? Space::CutDirection::RIGHT : Space::CutDirection::LEFT);

	//			// enter recover
	//			m_state = SwordSlashState::RECOVER;
	//			m_rotation_tween.Init(&m_sword, 8.0f, Tween::TWEEN_FUNCTION::EASE_IN_QUAD)
	//				.SetTweenTo(Tween::TWEEN_TYPE::ROTATION, Math::THREE_HALVES_PI);
	//		}
	//		else
	//		{
	//			m_frame_count--;
	//		}
	//		break;
	//	}
	//	case SwordSlashState::RECOVER:
	//	{
	//		m_sword.m_trail_shape = { Shape::ShapeType::SHAPE };
	//		m_blade->EndSwing();
	//		m_rotation_tween.Update();
	//		if (!m_rotation_tween.GetUse())
	//		{
	//			m_state = SwordSlashState::IDLE;
	//			m_blade->SetSize({ 0.0f, 40.0f });
	//			// TODO: exit
	//		}
	//		break;
	//	}
	//	}
	//}
	//void Sword::SwordStateSlash::Exit()
	//{
	//}
}
