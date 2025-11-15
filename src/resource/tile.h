#pragma once
#include "math/vector2.h"
#include "render/sprite_config.h"
#include "space_config.h"

namespace foo
{
	enum class TileType
	{
		NONE,
		SOLID,
		SOLID_SPACE,
		MAX
	};

	enum class SpritePresetName
	{
		DEFAULT,
		// player
		PLAYER_IDLE,
		PLAYER_EFFECT_NONE,
		PLAYER_MOVE,
		PLAYER_ATTACK_PRE_CHARGE,
		PLAYER_ATTACK_SLASH,
		PLAYER_ATTACK_SLASH_EFFECT,
		PLAYER_ATTACK_CHARGE,
		PLAYER_ATTACK_CHARGE_SLASH,
		PLAYER_ATTACK_CHARGE_SLASH_EFFECT,
		PLAYER_DIE,
		PLAYER_REVIVE,
		// enemy
		ENEMY_SHOOTER_OUTLINE,
		ENEMY_SHOOTER_EYE,
		ENEMY_SHOOTER_BODY,
		ENEMY_SHOOTER_ATTACK,
		// parallax
		PARALLAX_0,
		PARALLAX_1,
		PARALLAX_2,
		// item
		ITEM_CIRCLE,
		ITEM_FOCUS,
		ITEM_FAN_LEAF,
		ITEM_BORDER,
		ITEM_SQUARE,
		// text
		TEXT_MOVE,
		TEXT_CHARGE,
		TEXT_TURN,
		TEXT_SWITCH,
		TEXT_TAKE,
		TEXT_ATTACK,
		TEXT_PRESS_PLUS,
		TEXT_ENDING,
		// ui
		FADE,
		TITLE_MASK,
		MAX
	};

	struct TileView
	{
		SpriteConfigInput sprite_config_input;
		SpriteConfigOutput sprite_config_output;
	};

	struct Tile
	{
		bool active{ false };
	};

	class TileManager
	{
	public:
		void Initialize();
		void Finalize();
		const TileView& GetTileView(TileType type)
		{
			return m_views[static_cast<size_t>(type)];
		}
		const SpriteConfigInput& LoadSpriteConfig(SpritePresetName name);
	private:
		void InitializeSpritePreset();
		void InitializeSpritePresetPlayer(
			SpriteConfigInput* sprite_config_input, int frames, int start_row, int start_col);
		TileView m_views[static_cast<size_t>(TileType::MAX)];
		SpriteConfigInput m_sprite_presets[(size_t)SpritePresetName::MAX]{};
	};
}