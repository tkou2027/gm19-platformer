#pragma once
#include "render_component.h"
#include "render/sprite_config.h"

namespace foo
{
	class RenderSprites : public RenderComponent
	{
	public:
		RenderSprites(SpriteState* sprites, int sprite_num = 1);
		void Update() override;
		void UpdateRenderData() override;
	private:
		void UpdateSpriteRenderData(SpriteState& sprite_state);
		SpriteState* m_sprites{ nullptr };
		int m_sprite_num{ 0 };
	};
}