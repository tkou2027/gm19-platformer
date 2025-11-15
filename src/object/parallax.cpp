#include "parallax.h"
#include "component/render_sprites.h"
#include "global_context.h"
#include "render/render_system.h"
#include "resource/tile.h"
#include "player.h"

namespace foo
{
	Parallax::Parallax(bool auto_play) : m_auto_play(auto_play)
	{
		TileManager* tile_manager = g_global_context.m_tile_manager;
		for (int i = 0; i < (int)SpaceAccent::SPACE_ACCENT_MAX; i++)
		{
			SpritePresetName preset = (SpritePresetName)((int)SpritePresetName::PARALLAX_0 + i);
			m_sprites[i].Initialize(tile_manager->LoadSpriteConfig(preset));
		}
		SetRenderComponent(new RenderSprites(m_sprites, (int)SpaceAccent::SPACE_ACCENT_MAX));
	}

	void Parallax::Update()
	{
		if (m_auto_play)
		{
			m_offset_count++;
			m_offset_count = m_offset_count % (4 * SCREEN_WIDTH);
			for (int i = 0; i < (int)SpaceAccent::SPACE_ACCENT_MAX; i++)
			{
				m_sprites[i].uv_offset = {
					m_offset_count * 0.05f * i / SCREEN_WIDTH, // TODO
					0.0f,
				};
			}
			return;
		}
		Player* player = (Player*)GetNamedObject(ObjectName::PLAYER);
		const Vector2 player_pos{ player->GetTransform().GetTransform2D().position };

		Vector2 camera_offset = g_global_context.m_render_system->GetCamera().GetOffset();
		m_transform.SetPosition({ camera_offset });

		for (int i = 0; i < (int)SpaceAccent::SPACE_ACCENT_MAX; i++)
		{
			m_sprites[i].uv_offset = {
				camera_offset.x * 0.05f * i / SCREEN_WIDTH, // TODO
				camera_offset.y * 0.05f * i / SCREEN_HEIGHT,
			};
		}
	}
}
