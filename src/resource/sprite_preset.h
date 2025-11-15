#pragma once
#include "render/sprite_config.h"
namespace foo
{
	struct SpritePreset
	{
		enum class SpritePresetName
		{
			PLAYER_IDLE,
			COLORED_BLOCK,
			MAX
		};
		SpriteConfigInput m_sprite_configs[(size_t)SpritePresetName::MAX];
	};
}