#pragma once
#include "game_object.h"
#include "render/sprite_config.h"
#include "util/yoyo.h"

namespace foo
{
	class Fade;
	class Ending : public GameObject
	{
	public:
		Ending();
		void Update() override;
	private:
		Fade* m_fade;
		static constexpr int NUM_SPRITES{ 5 };
		SpriteState m_sprites[NUM_SPRITES];
		bool m_exit{ false };
		Yoyo m_yoyo{ 60 };
		Transform m_sprite_transform{};
		int m_item_countdown{ 0 };
	};
}