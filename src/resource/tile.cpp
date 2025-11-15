#include "tile.h"
#include "global_context.h"
// #include "render/texture.h"
#include "render/render_system.h"

namespace foo
{
	void TileManager::Initialize()
	{
		// init prototypes
		//m_prototypes[static_cast<int>(TileType::NONE)] = {
		//	TileType::NONE, TileInteractionType::NONE, TileViewType::NONE
		//};
		//m_prototypes[static_cast<int>(TileType::SOLID)] = {
		//	TileType::SOLID, TileInteractionType::SOLID, TileViewType::SOLID
		//};
		//m_prototypes[static_cast<int>(TileType::SOLID_SPACE)] = {
		//	TileType::SOLID_SPACE, TileInteractionType::SOLID_SPACE, TileViewType::SOLID_SPACE
		//};

		// load tile texture
		// m_texture_id = g_global_context.m_texture->LoadTexture("block_01.tga");
		InitializeSpritePreset();

		SpriteConfigInput sprite_config_solid{};
		sprite_config_solid.texture_path = "tiles.tga";
		sprite_config_solid.layer = RenderLayer::TILEMAP;
		m_views[static_cast<int>(TileType::SOLID)] = { sprite_config_solid };

		SpriteConfigInput sprite_config_solid_space{};
		sprite_config_solid_space.texture_path = "space_block.tga";
		sprite_config_solid_space.layer = RenderLayer::SPACE_BLOCK;
		sprite_config_solid_space.frame_cols = 2;
		sprite_config_solid_space.frame_total = 1;
		sprite_config_solid_space.color = Vector4(1.0f, 0.0f, 1.0f, 1.0f);
		m_views[static_cast<int>(TileType::SOLID_SPACE)] = { sprite_config_solid_space };

		for (int i = 1; i < static_cast<int>(TileType::MAX); i++)
		{
			g_global_context.m_render_system->LoadSpriteConfig(
				m_views[i].sprite_config_input
			);
			// TODO
			SpriteState state{};
			// default state
			state.Initialize(m_views[i].sprite_config_input);
			state.GetConfig(m_views[i].sprite_config_output);
		}
	}

	void TileManager::Finalize()
	{
		// g_global_context.m_texture->UnloadTexture(m_texture_id);
	}
	const SpriteConfigInput& TileManager::LoadSpriteConfig(SpritePresetName name)
	{
		g_global_context.m_render_system->LoadSpriteConfig(m_sprite_presets[(size_t)name]);
		return m_sprite_presets[(size_t)name];
	}
	void TileManager::InitializeSpritePreset()
	{
		SpriteConfigInput* sprite_config{ nullptr };

		// player ====
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_IDLE];
		InitializeSpritePresetPlayer(sprite_config, 1, 0, 0);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_EFFECT_NONE];
		InitializeSpritePresetPlayer(sprite_config, 1, 1, 3);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_MOVE];
		InitializeSpritePresetPlayer(sprite_config, 8, 0, 0);
		sprite_config->play_loop = true;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_ATTACK_PRE_CHARGE];
		InitializeSpritePresetPlayer(sprite_config, 3, 1, 0);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_ATTACK_SLASH];
		InitializeSpritePresetPlayer(sprite_config, 5, 2, 0);
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_ATTACK_SLASH_EFFECT];
		InitializeSpritePresetPlayer(sprite_config, 5, 3, 0);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_ATTACK_CHARGE];
		InitializeSpritePresetPlayer(sprite_config, 4, 1, 4);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_ATTACK_CHARGE_SLASH];
		InitializeSpritePresetPlayer(sprite_config, 5, 6, 0);
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_ATTACK_CHARGE_SLASH_EFFECT];
		InitializeSpritePresetPlayer(sprite_config, 5, 7, 0);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_DIE];
		InitializeSpritePresetPlayer(sprite_config, 8, 8, 0);

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PLAYER_REVIVE];
		InitializeSpritePresetPlayer(sprite_config, 7, 9, 0);

		// enemy ====
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ENEMY_SHOOTER_OUTLINE];
		sprite_config->texture_path = "enemy.tga";
		sprite_config->size = { 80.0f, 80.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 1;
		sprite_config->frame_default = 0;
		sprite_config->layer = RenderLayer::PLAYER;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ENEMY_SHOOTER_EYE];
		sprite_config->texture_path = "enemy.tga";
		sprite_config->size = { 64.0f, 64.0f };
		sprite_config->offset = { 0.0f, 0.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 2;
		sprite_config->frame_default = 1;
		sprite_config->layer = RenderLayer::PLAYER;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ENEMY_SHOOTER_BODY];
		sprite_config->texture_path = "enemy.tga";
		sprite_config->size = { 80.0f, 80.0f };
		sprite_config->offset = { 0.0f, 0.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 3;
		sprite_config->frame_default = 2;
		sprite_config->layer = RenderLayer::SPACE_MASK;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ENEMY_SHOOTER_ATTACK];
		sprite_config->texture_path = "enemy.tga";
		sprite_config->size = { 64.0f, 64.0f };
		sprite_config->offset = { 0.0f, 0.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 4;
		sprite_config->frame_default = 3;
		sprite_config->layer = RenderLayer::SPACE_MASK;

		// parallax ====
		const Vector2 parallax_uv_size{ 1.2f, 1.2f };
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PARALLAX_0];
		sprite_config->texture_path = "parallax_0.tga";
		sprite_config->size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		sprite_config->layer = RenderLayer::BACKGROUND_SPACE;
		sprite_config->space_accent = SpaceAccent::SPACE_ACCENT_0;
		sprite_config->uv_size = parallax_uv_size;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PARALLAX_1];
		sprite_config->texture_path = "parallax_01.tga";
		sprite_config->size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		sprite_config->layer = RenderLayer::BACKGROUND_SPACE;
		sprite_config->space_accent = SpaceAccent::SPACE_ACCENT_1;
		sprite_config->uv_size = parallax_uv_size;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::PARALLAX_2];
		sprite_config->texture_path = "parallax_02.tga";
		sprite_config->size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		sprite_config->layer = RenderLayer::BACKGROUND_SPACE;
		sprite_config->space_accent = SpaceAccent::SPACE_ACCENT_2;
		sprite_config->uv_size = parallax_uv_size;


		// items ====
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ITEM_CIRCLE];
		sprite_config->texture_path = "curcke.tga";
		sprite_config->size = { 120.0f, 120.0f };
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ITEM_FAN_LEAF];
		sprite_config->texture_path = "items.tga";
		sprite_config->size = { 48.0f, 48.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 1;
		sprite_config->frame_default = 0;
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ITEM_FOCUS];
		sprite_config->texture_path = "items.tga";
		sprite_config->size = { 100.0f, 100.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 2;
		sprite_config->frame_default = 1;
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ITEM_BORDER];
		sprite_config->texture_path = "items.tga";
		sprite_config->size = { 48.0f, 48.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 3;
		sprite_config->frame_default = 2;
		sprite_config->layer = RenderLayer::UI;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::ITEM_SQUARE];
		sprite_config->texture_path = "items.tga";
		sprite_config->size = { 100.0f, 100.0f };
		sprite_config->frame_cols = 4;
		sprite_config->frame_total = 4;
		sprite_config->frame_default = 3;
		sprite_config->layer = RenderLayer::ITEM;

		// ui ====
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::FADE];
		sprite_config->texture_path = "fade.tga";
		sprite_config->size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		sprite_config->uv_size = { 128.0f, 1.0f };
		sprite_config->layer = RenderLayer::UI;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TITLE_MASK];
		sprite_config->texture_path = "title.tga";
		sprite_config->size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		sprite_config->layer = RenderLayer::SPACE_MASK;
		sprite_config->space_id = SpaceId::SPACE_A;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_PRESS_PLUS];
		sprite_config->texture_path = "press_plus.tga";
		sprite_config->size = Vector2{ 256.0f, 16.0f } * 3.0f;
		sprite_config->layer = RenderLayer::UI;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_ENDING];
		sprite_config->texture_path = "ending.tga";
		sprite_config->size = Vector2{ 360.0f, 80.0f } * 3.0f;
		sprite_config->layer = RenderLayer::UI;

		// text
		float text_size = 80.0f;
		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_MOVE];
		sprite_config->texture_path = "text.tga";
		sprite_config->size = { text_size * 7, text_size };
		sprite_config->frame_rows = 5;
		sprite_config->frame_total = 1;
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_CHARGE];
		sprite_config->texture_path = "text.tga";
		sprite_config->size = { text_size * 2, text_size };
		sprite_config->frame_rows = 5;
		sprite_config->frame_total = 2;
		sprite_config->frame_default = 1;
		sprite_config->uv_size = { 2.0f / 7.0f, 1.0f };
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_TURN];
		sprite_config->texture_path = "text.tga";
		sprite_config->size = { text_size * 7, text_size };
		sprite_config->frame_rows = 5;
		sprite_config->frame_total = 3;
		sprite_config->frame_default = 2;
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_SWITCH];
		sprite_config->texture_path = "text.tga";
		sprite_config->size = { text_size * 2, text_size };
		sprite_config->frame_rows = 5;
		sprite_config->frame_total = 4;
		sprite_config->frame_default = 3;
		sprite_config->uv_size = { 2.0f / 7.0f, 1.0f };
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_TAKE];
		sprite_config->texture_path = "text.tga";
		sprite_config->size = { text_size, text_size };
		sprite_config->frame_rows = 5;
		sprite_config->frame_total = 5;
		sprite_config->frame_default = 4;
		sprite_config->uv_size = { 1.0f / 7.0f, 1.0f };
		sprite_config->layer = RenderLayer::ITEM;

		sprite_config = &m_sprite_presets[(size_t)SpritePresetName::TEXT_ATTACK];
		sprite_config->texture_path = "text.tga";
		sprite_config->size = { text_size * 3, text_size };
		sprite_config->frame_rows = 5;
		sprite_config->frame_total = 5;
		sprite_config->frame_default = 4;
		sprite_config->uv_size = { 3.0f / 7.0f, 1.0f };
		sprite_config->layer = RenderLayer::ITEM;
	}

	void TileManager::InitializeSpritePresetPlayer(
		SpriteConfigInput* sprite_config, int frames, int start_row, int start_col)
	{
		const Vector2 player_sprite_size{ 384.0f, 384.0f };
		const Vector2 player_sprite_offset{ 0.0f, -96.0f };
		constexpr int player_cols{ 8 };
		constexpr int plyer_rows{ 10 };
		const Vector2 player_uv_size{ 1.0f / player_cols, 1.0f / plyer_rows };
		sprite_config->texture_path = "player_sheet.tga";
		sprite_config->size = player_sprite_size;
		sprite_config->offset = player_sprite_offset;
		sprite_config->frame_cols = frames;
		sprite_config->frame_total = frames;
		sprite_config->layer = RenderLayer::PLAYER;
		sprite_config->play_speed_scale = 4;

		sprite_config->uv_offset = { player_uv_size.x * start_col, player_uv_size.y * start_row };
		sprite_config->uv_size = { player_uv_size.x * frames, player_uv_size.y };
	}
}