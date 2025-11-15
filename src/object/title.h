#pragma once
#include "game_object.h"
#include "render/sprite_config.h"
#include "util/yoyo.h"

namespace foo
{
	class Fade;
	class Title : public GameObject
	{
	public:
		Title();
		void Update() override;
	private:
		Fade* m_fade;
		static constexpr int NUM_SPRITES{ 2 };
		SpriteState m_sprites[NUM_SPRITES];
		bool m_exit{ false };
		Yoyo m_yoyo{ 60 };
	};
}