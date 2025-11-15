#include "fan.h"

#include "component/render_sprite.h"
#include "trail_manager.h"
#include "player.h"
#include "global_context.h"
#include "resource/tile.h"
#include "component/render_sprites.h"

// TODO
#include "particle_system.h"
#include "util/random.h"
#include "platform/controller.h";
#include "platform/sound.h"
#include "hint.h"

namespace foo
{
	Fan::Fan(int scope_id) : m_scope_id( scope_id )
	{
		m_blade_size = { 300.0f, 32.0f }; // TODO
		// blade
		float rotation_per_blade{ Math::TWO_PI / NUM_BLADE };
		TileManager* tile_manager = g_global_context.m_tile_manager;
		for (int i = 0; i < NUM_BLADE; i++)
		{
			// transform
			float rotation = i * rotation_per_blade;
			m_parts_transform[i].SetRotation(rotation);
			float center_dist = m_blade_size.x * 0.5f;
			m_parts_transform[i].SetPosition({ center_dist * cosf(rotation), center_dist * sinf(rotation) });
			m_transform.AddChild(m_parts_transform + i);
			// render
			if (i == 0)
			{
				m_parts_sprite[i].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_SQUARE));
			}
			else
			{
				m_parts_sprite[i].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_FAN_LEAF));
			}
			m_parts_sprite[i].size = m_blade_size; // TODO
			m_parts_sprite[i].layer = RenderLayer::AFTER_EFFECT; // TODO
			m_parts_sprite[i].transform = m_parts_transform + i;
		}
		// center
		m_center_sprite_id = NUM_BLADE;
		m_parts_sprite[m_center_sprite_id].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_CIRCLE));
		m_parts_sprite[m_center_sprite_id].layer = RenderLayer::AFTER_EFFECT;
		m_parts_sprite[m_center_sprite_id].color = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };

		// focus
		m_focus_sprite_id = NUM_BLADE + 1;
		m_parts_sprite[m_focus_sprite_id].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_FOCUS));
		m_parts_sprite[m_focus_sprite_id].layer = RenderLayer::AFTER_EFFECT;
		m_parts_sprite[m_focus_sprite_id].color = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
		m_parts_transform[m_focus_sprite_id].SetRotation(Math::PI * 0.25f);
		m_transform.AddChild(m_parts_transform + m_focus_sprite_id);
		m_parts_sprite[m_focus_sprite_id].transform = m_parts_transform + m_focus_sprite_id;

		// hint
		SetRenderComponent(new RenderSprites(m_parts_sprite, NUM_SPRITES));
		m_fan_manager = (FanManager*)GetNamedObject(ObjectName::FAN_MANAGER);
		m_player = (Player*)GetNamedObject(ObjectName::PLAYER);
		// trail manager
		m_trail_manager = (TrailManager*)GetNamedObject(ObjectName::TRAIL_MANAGER);

		Initialize();
	}

	void Fan::Initialize()
	{
		// m_transform.SetRotation(Math::PI * 0.25f);
		m_state = FanState::IDLE;
		m_tween.SetUnused();
	}

	void Fan::Update()
	{
		switch (m_state)
		{
		case FanState::IDLE:
		{
			UpdateIdle();
			m_parts_sprite[m_focus_sprite_id].transform->SetRotation(m_yoyo.GetT() * Math::PI * 0.25f);
			break;
		}
		case FanState::CHARGE:
		{
			UpdateCharge();
			break;
		}
		case FanState::BOUNCE:
		{
			UpdateBounce();
			m_parts_sprite[m_focus_sprite_id].transform->SetRotation(m_yoyo.GetT() * Math::PI * 0.125f);
			break;
		}
		}
		
		m_yoyo.Update();
	}

	void Fan::OnCancelFocus()
	{
		m_status.focused = false;
		m_status.charge = 0;
		switch (m_state)
		{
		case FanState::IDLE:
		case FanState::CHARGE:
		{
			ResetState();// ?
			break;
		}
		case FanState::BOUNCE:
		{
			ExitBounce();
			break;
		}
		}
		m_state = FanState::IDLE;
	}

	void Fan::SetActive()
	{
		Initialize();
		GameObject::SetActive();
	}

	void Fan::SetIdle()
	{
		// TODO
		if (m_state == FanState::CHARGE || m_state == FanState::BOUNCE)
		{
			m_fan_manager->CancelFocus();
		}
		OnCancelFocus();
		GameObject::SetActive();
	}

	void Fan::SetRemoved()
	{
		// TODO
		if (m_state == FanState::CHARGE || m_state == FanState::BOUNCE)
		{
			m_fan_manager->CancelFocus();
		}
		OnCancelFocus();
		GameObject::SetRemoved();
	}

	void Fan::UpdateIdle()
	{
		// TODO controller
		m_transform.SetRotation(m_transform.GetRotation() + 0.002f);

		bool focused = CheckFocused();
		if (focused && !m_status.focused)
		{
			m_parts_sprite[m_focus_sprite_id].color = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };

			// TODO
			if (m_scope_id != 0)
			{
				Hint* hint = (Hint*)GetNamedObject(ObjectName::HINT);
				hint->ShowHint(Hint::HintType::CHARGE, m_transform.GetTransform2D().position);
			}
		}
		else if (!focused && m_status.focused)
		{
			m_parts_sprite[m_focus_sprite_id].color = Vector4{ 1.0f, 1.0f, 1.0f, 0.0f };
		}
		m_status.focused = focused;
		if (!focused)
		{
			return;
		}
		const Controller* controller = g_global_context.m_controller;
		if (controller->GetControllerTrigger(Button::BUTTON_L) || controller->GetControllerTrigger(Button::BUTTON_R))
		{
			m_state = FanState::CHARGE;
		}
	}

	void Fan::UpdateCharge()
	{
		const Controller* controller = g_global_context.m_controller;
		if (controller->GetControllerPress(Button::BUTTON_L) || controller->GetControllerPress(Button::BUTTON_R))
		{
			++m_status.charge;
		}
		else
		{
			--m_status.charge;
		}
		for (int i = 0; i < 1; i++)
		{
			float t = (float)m_status.charge / m_config.charge_countdown;
			const Vector4 white{ 1.0f, 1.0f, 1.0f, 1.0f };
			m_parts_sprite[i].color =
				white * (1.0f - t)
				+ g_space_config.GetSpaceProp(m_space_id).color * t;
		}
		if (m_status.charge == 0)
		{
			m_state = FanState::IDLE;
		}
		else if (m_status.charge >= m_config.charge_countdown)
		{
			m_fan_manager->ApplyFocus(this);

			m_status.charge = 0;
			m_state = FanState::BOUNCE;
			Burst();
		}
	}

	void Fan::UpdateBounce()
	{
		const Controller* controller = g_global_context.m_controller;

		// update rotation
		if (m_tween.GetUse())
		{
			m_tween.Update();
		}
		else
		{
			float rotation_z = controller->GetControllerLeftAngleToOffset().x * Math::TWO_PI * -12.0f;
			float rotation_delta = rotation_z - m_rotation_offset;
			if (!(controller->GetControllerPress(Button::BUTTON_L) || controller->GetControllerPress(Button::BUTTON_R)))
			{
				rotation_delta = Math::ClampMagnitude(rotation_delta, m_config.bounce_rotation_speed);
			}
			m_transform.SetRotation(m_transform.GetRotation() + rotation_delta);
		}
		m_rotation_offset = controller->GetControllerLeftAngleToOffset().x * Math::TWO_PI * -12.0f;

		float rotation = m_transform.GetTransform2D().rotation;
		m_trail->SetNormal(Vector2{ sin(rotation), -cos(rotation) });
	}

	void Fan::Burst()
	{
		// vibe
		Controller* controller = g_global_context.m_controller;
		controller->SetControllerLeftVibration(6);
		controller->SetControllerRightVibration(6);

		// screen shake
		Camera& camera = g_global_context.m_render_system->GetCamera();
		camera.TriggerShake({ 0.0f, 8.0f });

		for (int i = 0; i < NUM_BLADE; i++)
		{
			m_parts_sprite[i].color = g_space_config.GetSpaceProp(m_space_id).color;
		}
		m_parts_sprite[m_center_sprite_id].color = g_space_config.GetSpaceProp(m_space_id).color;
		m_parts_sprite[m_focus_sprite_id].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		m_tween.Init(this, 60.0f, Tween::TWEEN_FUNCTION::EASE_OUT_BOUNCE)
			.SetTweenToDelta(Tween::TWEEN_TYPE::ROTATION, Math::PI);
		GenTrail();

		// TODO
		Hint* hint = (Hint*)GetNamedObject(ObjectName::HINT);
		hint->ShowHint(Hint::HintType::TURN, m_transform.GetTransform2D().position);
	}

	void Fan::GenTrail()
	{
		float rotation = m_transform.GetTransform2D().rotation;
		const Vector2 normal{ sin(rotation), -cos(rotation) };
		TrailObjectLine::TrailObjectLineConfig config
		{
			m_transform.GetTransform2D().position,
			normal
		};
		TrailObjectLine* trail = new TrailObjectLine(m_space_id, m_scope_id, config);
		m_trail = trail;
		m_trail_manager->Add(trail);
	}

	void Fan::ResetState()
	{
		m_state = FanState::IDLE;
		m_status = {};
		for (int i = 0; i < NUM_BLADE; i++)
		{
			m_parts_sprite[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		}
		m_parts_sprite[m_center_sprite_id].color = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	void Fan::ExitBounce()
	{
		Controller* controller = g_global_context.m_controller;
		controller->SetControllerLeftVibration(2);
		controller->SetControllerRightVibration(2);

		if (m_trail)
		{
			m_trail->Destroy();
			m_trail = nullptr;
		}
		ResetState();
	}

	bool Fan::CheckFocused()
	{
		if (m_no_focus) { return false;  }
		const Vector2& pos{ m_transform.GetTransform2D().position };
		return m_player->IfPlayerInDistance(pos, m_config.lock_distance);
	}

	// manager =======
	FanManager::FanManager()
	{
		m_player = (Player*)GetNamedObject(ObjectName::PLAYER);
		// set named object
		m_name = ObjectName::FAN_MANAGER;
		SetNamedObject(m_name, this);
	}

	void FanManager::Update()
	{
		CheckCancelFocus();
	}

	void FanManager::ApplyFocus(Fan* fan)
	{
		if (m_active_fan)
		{
			m_active_fan->OnCancelFocus();
		}
		m_active_fan = fan;
	}

	void FanManager::CancelFocus()
	{
		ApplyFocus(nullptr);
	}

	void FanManager::CheckCancelFocus()
	{
		if (!m_player->GetPlayerActive())
		{
			ApplyFocus(nullptr);
		}
		// TODO: check scope id
	}
}
