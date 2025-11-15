#include "fade.h"

#include "component/render_sprites.h"
#include "global_context.h"
#include "render/render_system.h"
#include "resource/tile.h"
#include "util/tween.h"

namespace foo
{
	Fade::Fade(const FadeConfig& config) : m_config(config)
	{
		TileManager* tile_manager = g_global_context.m_tile_manager;
		for (int i = 0; i < NUM_SPRITES; i++)
		{
			m_sprites[i].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::FADE));
			m_sprites[i].space_id = m_config.space_id;
			if (m_config.mask)
			{
				m_sprites[i].layer = RenderLayer::SPACE_MASK;
			}
			else
			{
				m_sprites[i].color = g_space_config.GetSpaceProp(m_config.space_id).color;
			}
		}
		m_sprites[1].uv_offset.x = 0.5f;
		SetRenderComponent(new RenderSprites(m_sprites, NUM_SPRITES));

		EnterIdle();
	}
	void Fade::Update()
	{
		switch (m_state)
		{
		case FadeState::FADE_IN:
		{
			UpdateTransform();
			UpdateFadeIn();
			break;
		}
		case FadeState::FADE_OUT:
		{
			UpdateTransform();
			UpdateFadeOut();
			break;
		}
		}
	}
	void Fade::StartFadeIn(bool fade_in_out)
	{
		m_countdown = m_config.duration;
		m_fade_in_out = fade_in_out;
		m_fade_out_in = false;
		m_state = FadeState::FADE_IN;

		m_sprites[0].offset.y = 0;
		m_sprites[1].offset.y = 0;

		m_render_component->SetActive(true);
	}

	void Fade::StartFadeOut(bool fade_out_in)
	{
		m_countdown = m_config.duration;
		m_fade_out_in = fade_out_in;
		m_fade_in_out = false;
		m_state = FadeState::FADE_OUT;

		m_sprites[0].offset.y = -SCREEN_HEIGHT;
		m_sprites[1].offset.y = SCREEN_HEIGHT;

		m_render_component->SetActive(true);
	}

	bool Fade::IsIdle() const
	{
		return m_state == FadeState::IDLE;
	}

	void Fade::EnterIdle()
	{
		m_render_component->SetActive(false);
		m_state = FadeState::IDLE;
		m_fade_in_out = false;
		m_fade_out_in = false;
	}

	void Fade::UpdateFadeIn()
	{
		if (m_countdown == 0)
		{
			if (m_fade_in_out)
			{
				m_fade_in_out = false;
				UpdateFadeOut();
			}
			else
			{
				EnterIdle();
			}
		}
		float t = 1.0f - (float)m_countdown / m_config.duration;
		Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_OUT_QUAD, t);
		m_sprites[0].offset.y = -SCREEN_HEIGHT * t;
		m_sprites[1].offset.y = SCREEN_HEIGHT * t;

		m_countdown--;
	}

	void Fade::UpdateFadeOut()
	{
		if (m_countdown == 0)
		{
			if (m_fade_out_in)
			{
				m_fade_out_in = false;
				UpdateFadeIn();
			}
			else
			{
				EnterIdle();
			}
			return;
		}
		float t = 1.0f - (float)m_countdown / m_config.duration;
		t = Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_IN_QUAD, t);
		m_sprites[0].offset.y = -SCREEN_HEIGHT * (1.0f - t);
		m_sprites[1].offset.y = SCREEN_HEIGHT * (1.0f - t);

		m_countdown--;
	}

	void Fade::UpdateTransform()
	{
		Vector2 camera_offset = g_global_context.m_render_system->GetCamera().GetOffset();
		m_transform.SetPosition({ camera_offset });
	}
}