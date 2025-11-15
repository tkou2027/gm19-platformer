#include "hint.h"

#include "component/render_sprites.h"
#include "global_context.h"
#include "resource/tile.h"

namespace foo
{
	Hint::Hint()
	{
		for (int i = 0; i < NUM_SPRITES; i++)
		{
			m_sprites[i].active = false;
			m_sprites[i].color = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
			m_sprites[i].transform = m_sprite_transform + i;
		}
		for (int i = 0; i < (int)HintType::MAX; i++)
		{
			m_hint_end[i] = false;
		}

		SetRenderComponent(new RenderSprites(m_sprites, NUM_SPRITES));

		m_name = ObjectName::HINT;
		SetNamedObject(m_name, this);
	}

	void Hint::Update()
	{
		switch (m_hint_state)
		{
		case HintState::FADE_IN:
		{
			if (m_countdown == 0)
			{
				m_hint_state = HintState::ACTIVE;
				m_countdown = m_config.active_countdown;
				break;
			}
			float t = 1.0f - (float)m_countdown / m_config.fade_in_countdown;
			for (int i = 0; i < NUM_SPRITES; i++)
			{
				m_sprites[i].color.w = t;
			}
			m_countdown--;
			break;
		}
		case HintState::ACTIVE:
		{
			if (m_countdown == 0)
			{
				EnterFadeOut();
				break;
			}
			float t = 1.0f - (float)m_countdown / m_config.fade_in_countdown;
			if (m_hint_type == HintType::TURN)
			{
				m_sprite_transform[1].SetRotation(sinf(t) * 0.5);
			}
			m_countdown--;
			break;
		}
		case HintState::FADE_OUT:
		{
			if (m_countdown == 0)
			{
				EndHint();
				break;
			}
			float t = 1.0f - (float)m_countdown / m_config.fade_in_countdown;
			for (int i = 0; i < NUM_SPRITES; i++)
			{
				m_sprites[i].color.w = 1 - t;
			}
			m_countdown--;
			break;
		}
		}
	}

	void Hint::ShowHint(HintType type, const Vector2& position)
	{
		if (m_hint_end[(size_t)type] && !(type == HintType::TAKE && m_hint_state == HintState::NONE))
		{
			return;
		}
		m_hint_end[(size_t)type] = true;
		
		m_hint_type = type;
		m_countdown = 1200;
		TileManager* tile_manager = g_global_context.m_tile_manager;
		switch (type)
		{
		case HintType::MOVE:
			m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_MOVE));
			break;
		case  HintType::CHARGE:
			m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_CHARGE));
			break;
		case  HintType::TURN:
			m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_TURN));
			m_sprites[1].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_SWITCH));
			m_sprite_transform[1].SetPosition({ position + Vector2{ 20.0f, -80.0f } });
			break;
		case  HintType::ATTACK:
			m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_ATTACK));
			break;
		case  HintType::TAKE:
			m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_TAKE));
			break;
		default:
			break;
		}
		for (int i = 0; i < NUM_SPRITES; i++)
		{
			m_sprites[i].color = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
		}
		Vector2 pos_offset = position + Vector2{ 0.0f, -80.0f };
		m_sprite_transform[0].SetPosition({ pos_offset });
		
		EnterFadeIn();
	}

	void Hint::StopHint(HintType type)
	{
		if (m_hint_type != type)
		{
			return;
		}
		EnterFadeOut();
	}

	void Hint::EnterFadeIn()
	{
		m_countdown = m_config.fade_in_countdown;
		m_hint_state = HintState::FADE_IN;
		for (int i = 0; i < NUM_SPRITES; i++)
		{
			m_sprites[i].color.w = 0.0f;
		}
		m_render_component->SetActive(true);
	}

	void Hint::EnterFadeOut()
	{
		m_countdown = m_config.fade_out_countdown;
		m_hint_state = HintState::FADE_OUT;
	}

	void Hint::EndHint()
	{
		for (int i = 0; i < NUM_SPRITES; i++)
		{
			m_sprites[i].active = false;
		}
		m_render_component->SetActive(false);
		m_hint_state = HintState::NONE;
	}
}
