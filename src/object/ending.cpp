#include "ending.h"
#include "parallax.h"

#include "global_context.h"
#include "platform/sound.h"
#include "platform/controller.h"
#include "scene/scene.h"
#include "game_status.h"
#include "fade.h"
#include "rain.h"
#include "resource/tile.h"
#include "component/render_sprites.h"
#include "util/tween.h"

namespace foo
{
	Ending::Ending()
	{
		Parallax* m_parallax = new Parallax(true);
		AddObject(m_parallax);

		Rect bbox{ {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT + 200.0f}, {0, 200.0f} };

		Rain* rain = new Rain(0, bbox, SpaceId::SPACE_A);
		AddObject(rain);
		rain->SetActive();

		m_fade = new Fade({});
		AddObject(m_fade);


		// sprites
		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_ENDING));
		m_sprites[1].offset = { 200.0f, 100.0f };
		m_sprites[1].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_PRESS_PLUS));
		m_sprites[1].offset.y = 200.0f;

		// score
		m_sprite_transform.SetRotation(Math::PI * 0.25f);
		for (int i = 2; i < 2 + GameStatus::ITEM_NUM; i++)
		{
			bool get = g_global_context.m_game_status->GetItemGet(static_cast<size_t>(i - 2));
			m_sprites[i].Initialize(tile_manager->LoadSpriteConfig(get ? SpritePresetName::ITEM_SQUARE : SpritePresetName::ITEM_BORDER));
			m_sprites[i].offset = { -80.0f + i * 140.0f, -40.0f };
			m_sprites[i].size = { 60.0f, 60.0f };
			m_sprites[i].layer = RenderLayer::UI;
			m_sprites[i].transform = &m_sprite_transform;
		}

		SetRenderComponent(new RenderSprites(m_sprites, NUM_SPRITES));
	}
	void Ending::Update()
	{
		for (int i = 2; i < 5; i++)
		{
			float t = (m_item_countdown - 15.0f * (i - 2)) / 60.0f ;
			t = t < 0 ? 0 : (t > 1 ? 1 : t);
			m_sprites[i].size = Vector2{ 80.0f, 80.0f } *Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_IN_OUT_BACK, t);
		}
		m_item_countdown++;
		if (m_item_countdown > 90)
		{
			m_item_countdown = 90;
		}

		if (!m_exit)
		{
			m_sprites[1].offset.y = 200.0f + m_yoyo.GetT() * 10.0f;
			m_yoyo.Update();
			if (g_global_context.m_controller->
				GetControllerTrigger(Button::BUTTON_PLUS))
			{
				m_exit = true;
				m_fade->StartFadeOut();
				g_global_context.m_sound->PlaySe(SeName::CLICK); // スタート音再生
			}
		}
		else
		{
			m_sprites[1].color.w = m_yoyo.GetT();
			if (m_fade->IsIdle())
			{
				g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_TITLE);
				g_global_context.m_sound->StopBgm();
			}
		}

	}
}
	
