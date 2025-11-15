#include "title.h"
#include "parallax.h"

#include "global_context.h"
#include "platform/sound.h"
#include "platform/controller.h"
#include "scene/scene.h"
#include "fade.h"
#include "rain.h"
#include "resource/tile.h"
#include "component/render_sprites.h"

namespace foo
{
	Title::Title()
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
		m_sprites[0].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TITLE_MASK));
		m_sprites[1].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::TEXT_PRESS_PLUS));
		m_sprites[1].offset.y = 200.0f;
		SetRenderComponent(new RenderSprites(m_sprites, NUM_SPRITES));
	}
	void Title::Update()
	{
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
				g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_GAME);
				g_global_context.m_sound->StopBgm();
			}
		}
	}
}

