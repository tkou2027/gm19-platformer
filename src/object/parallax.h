#pragma once
#include "game_object.h"
#include "render/sprite_config.h"
#include "resource/space_config.h"

namespace foo
{
	class Parallax : public GameObject
	{
	public:
		Parallax(bool auto_play = false);
		void Update() override;
	private:
		SpriteState m_sprites[static_cast<size_t>(SpaceAccent::SPACE_ACCENT_MAX)];
		bool m_auto_play{ false };
		int m_offset_count{ 0 };
	};
}